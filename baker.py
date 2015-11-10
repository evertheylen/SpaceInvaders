#!/usr/bin/python3

import subprocess # call

import os  # listdir, path, getcwd
import argparse
import glob  # glob.glob

import pdb  # debugging, pdb.set_trace()

import copy
import csv

import datetime
MAX_DATE = datetime.datetime(9999, 12, 31)
MIN_DATE = datetime.datetime(1,1,1)

global all_units
# key is location!!
all_units = {}

global all_todos
# for a given todo t: all_todos[t] = t
all_todos = {}
# I agree this is weird. It's basically like a set only with an easier way
# to retrieve the value already in there, so we don't create the same Todo
# (as defined by __eq__) two times (different objects in memory)

hard_debug = False
def dbg_print(*args):
    if hard_debug:
        print(*args)

# ----------------------------------------------------
# COLORS YAY
endc = '\033[0m'  # Resets all ANSI attributes
 
red = (255, 0, 0)
green = (0, 255, 0)
blue = (0, 0, 255)
 
cyan = (0, 255, 255)
pink = (255, 0, 255)
yellow = (255, 255, 0)

orange = (255, 165, 0)

black = (0, 0, 0)
white = (255, 255, 255)
 
def _rgb(rf,gf,bf,rb=0,gb=0,bb=0):
    # red front, ..., blue back
    return "\033[1;38;2;{};{};{};48;2;{};{};{}m".format(rf,gf,bf,rb,gb,bb)

def rgb(f, b=black):
    return _rgb(f[0], f[1], f[2], b[0], b[1], b[2])

def rgbtext(s, f=red, b=black):
    return rgb(f,b) + s + endc



# -------------------------------------------------------
# Logging

class IndentWriter:
    def __init__(self, debug):
        self.debug = debug
        self.indent = 0
        self.byte_cache = []
        
    def writeline(self, l):
        self.byte_cache.clear()
        print(self.indent*"  " + l)
        
    
    def debugline(self, l):
        if self.debug:
            self.writeline("DBG: " + l)
    
    def writebytes(self, b):
        s = b.decode("utf-8")[:-1]
        if s.isspace() or s == "":
            self.byte_cache.append(s)
        else:
            for cs in self.byte_cache:
                print(self.indent*"  " + cs)
            self.byte_cache.clear()
            print(self.indent*"  " + s)
    
    def tab(self):
        # max indent of 10 (20 spaces)
        self.indent = min(10, self.indent+1)
    
    def untab(self):
        self.indent = max(0, self.indent-1)



# ------------------------------------------------------
# Exceptions

class BakeError(Exception):
    pass

class ConfigError(BakeError):
    def __init__(self, where, key, info=""):
        self.where = where
        self.key = key
        self.info = info
    
    def __str__(self):
        return rgbtext(("Error in configuration: '{s.where}' should contain a proper value for '{s.key}'." + (" ("+self.info+")" if self.info != "" else "")).format(s=self), orange)

class BuildError(BakeError):
    def __init__(self, todo, info=""):
        self.todo = todo
        self.info = info
        
    def __str__(self):
        return rgbtext(("Error while building todo '{s.todo}'." + (" ("+self.info+")" if self.info != "" else "")).format(s=self), red)


# ------------------------------------------------------
# Database

# the 'database' is actually a csv, like this:
# A,worker,action,file1,file2
# B,worker,action,file3,file4

class Database:
    def __init__(self, filename):
        self.filename = filename
        self.values = {}
        if os.path.isfile(filename):
            with open(filename) as f:
                reader = csv.reader(f, delimiter=' ', quotechar='|')
                for row in reader:
                    if len(row) >= 3:
                        self.values[(row[0],row[1],row[2])] = row[3:]
        
    def write(self):
        with open(self.filename, 'w') as f:
            writer = csv.writer(f, delimiter=' ', quotechar='|', quoting=csv.QUOTE_MINIMAL)
            for k,v in self.values.items():
                writer.writerow(list(k) + v)
    
    @classmethod
    def tuplify(cls, todo):
        return (todo.unit.name, todo.worker.shortname, todo.action)
    
    def get_row(self, todo):
        return self.values[Database.tuplify(todo)]
    
    def set_row(self, todo, files):
        if files is not None:
            self.values[Database.tuplify(todo)] = files
    
    def __contains__(self, todo):
        return Database.tuplify(todo) in self.values 
    
    def append_row(self, todo, extra_files):
        t = Database.tuplify(todo)
        if t in self.values:
            self.values[t].append(extra_files)
        else:
            self.values[t] = extra_files
    
        
    
# ------------------------------------------------------
# Config

class ConfigDict(dict):
    pass


def isConfig(e):
    return isinstance(e, ConfigDict)

class Config:  
    def __getitem__(self, key):
        for invlayer in self.inv_layers():
            if key in invlayer:
                inve = invlayer[key]
                if isConfig(inve):
                    return NestedConfig(self, key)
        
        for layer in self.layers():
            if key in layer:
                return layer[key]
        
        raise KeyError(key)
    
    def keys(self):
        kys = set()
        for layer in self.layers():
            kys.update(layer.keys())
        yield from kys
    
    __iter__ = keys
    
    def items(self):
        for k in self.keys():
            yield (k, self[k])
    
    def values(self):
        for k in self.keys():
            yield self[k]
    
    def layers_empty(self):
        for i in self.layers:
            return False
        return True


class NestedConfig(Config):
    def __init__(self, parent, parentkey, toplayers=[]):
        self.parent = parent
        self.parentkey = parentkey
        self.toplayers = toplayers
    
    def layers(self):
        yield from self.toplayers
        for parentlayer in self.parent.layers():
            if self.parentkey in parentlayer:
                yield parentlayer[self.parentkey]
    
    def inv_layers(self):
        yield from inverse_from(self.toplayers)
        for parentlayer in self.parent.inv_layers():
            if self.parentkey in parentlayer:
                yield parentlayer[self.parentkey]
    
    def new_extra_layer(self, extra):
        return NestedConfig(self.parent, self.parentkey, [extra] + self.toplayers)
        

class HardConfig(Config):
    def __init__(self, real_layers):
        self.real_layers = real_layers
    
    def layers(self):
        yield from self.real_layers
    
    def inv_layers(self):
        yield from inverse_from(self.real_layers)
        
    def add_low_priority(self, layer):
        self.real_layers.append(layer)
    
    def add_high_priority(self, layer):
        self.real_layers.insert(0, layer)
    
    def new_extra_layer(self, extra):
        return HardConfig([extra] + self.real_layers)



# ------------------------------------------------------
# Utilities

def inverse_from(l):
    i = len(l)-1
    while i >= 0:
        yield l[i]
        i -= 1

def escape_path(s):
    #TODO possibly replace more stuff than a space
    # windows support?
    news = s.replace(" ", "\\ ")
    return news
    

def call(s, writer, cwd=None):
    writer.debugline(">> %s [cwd=%s]"%(s, cwd))
    # if shell=True, it is recommended to pass the command as a string, rather than as a list
    p = subprocess.Popen(s, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, cwd=cwd)
    while True:
        retcode = p.poll()
        writer.writebytes(p.stdout.readline())
        # stderr??
        if retcode is not None:
            for l in p.stdout.readlines():
                writer.writebytes(l)
            return retcode

def check_call(s, writer, todo, info="", cwd=None):
    ret = call(s, writer, cwd)
    if ret != 0:
        raise BuildError(todo, info)
    
    
def drop_ext(s):
    return s[:s.rfind(".")]

def there_exists(collection, condition):
    for i in collection:
        if condition(i):
            return True
    return False

# returns None if key not in dct, dct[key] otherwise
def get(key, dct):
    if key in dct:
        return dct[key]
    return None

def empty(e):
    return (e is None or e == "")

def pick_nonempty(*args):
    for i in args:
        if not empty(i):
            return i

def exec_file(location, loc=locals()):
    #print("searching ",location)
    if os.path.isfile(location):
        f = open(location)
        fc = compile(f.read(), location.split("/")[-1], "exec")
        exec(fc, globals(), loc)
        return True
    return False


def dates_from_files(filenames):
    #print("dates_from_files got " + repr(filenames))
    for fn in filenames:
        if os.path.isfile(fn) or os.path.isdir:
            yield datetime.datetime.fromtimestamp(os.path.getmtime(fn))
        else:
            yield MIN_DATE

# thanks http://stackoverflow.com/questions/1158076/implement-touch-using-python
def touch(fname, mode=0o666, dir_fd=None, **kwargs):
    if os.path.isfile(fname):
        flags = os.O_CREAT | os.O_APPEND
        #print("touching " + fname)
        with os.fdopen(os.open(fname, flags=flags, mode=mode, dir_fd=dir_fd)) as f:
            os.utime(f.fileno() if os.utime in os.supports_fd else fname,
                dir_fd=None if os.supports_fd else dir_fd, **kwargs)

def parse_worker(worker, config, worker_classes):
    configs = []
    
    # Check own config
    for c in worker.needs_config:
        configs.append(config[c])
        
    return worker(*configs)


def add_to_list_in_dict(dct, key, extra):
    if key not in dct:
        dct[key] = extra
    else:
        dct[key] += extra

# -----------------------------------------------------------
# Units

def create_unit(uname, config):
    if uname.startswith("@"):
        collection = True
        uloc = os.path.abspath(uname[1:])
    else:
        collection = False
        uloc = os.path.abspath(uname)
    
    # TODO weirdness ensues when trying to define the same dir as both collection and unit
    if uloc in all_units:
        return all_units[uloc]
    else:
        if collection:
            u = CollectionUnit(uname, config)
        else:
            u = Unit(uname)
        all_units[u.location] = u
        u.scan(config)
        return u


# not an actual unit
class BaseUnit:
    def __init__(self, name):
        self.name = name


class Unit(BaseUnit):
    def __init__(self, name):
        self.name = name
        self.location = os.path.abspath(name)
        # TODO set sensible defaults
        self.data = {
            "worker": "",
            "dependencies": {},
        }
        self.worker = None
        self.deps = {}  # for each action, certain deps
        
        self.files = None
    
    def __eq__(self, other):
        return os.path.samefile(self.location, other.location)
    
    def __hash__(self):
        return hash(self.location)
    
    def scan(self, config):
        self.parse_bakeme(config)
        self.parse_deps(config)
    
    def parse_bakeme(self, config):
        # checks all files for baker-related stuff
        # these files are either bake_me.py or marked comments
        # within sources with the proper extension
        self.files = []
        
        # first, bake_me.py
        bake_me_loc = os.path.join(self.location, "bake_me.py")
        try:
            if exec_file(bake_me_loc, self.data):
                self.files.append(bake_me_loc)
        except Exception as e:
            raise ConfigError(bake_me_loc, "whatever", str(e))
        
        # then, check all files
        # TODO more efficient :P
        for pat in config["scanned_files"]:
            for f_loc in glob.glob(os.path.join(self.location, pat)):
                self.files.append(f_loc)
                f = open(f_loc)
                bake_code = ""
                copy = False
                for line in f:
                    if "[bake me]" in line:
                        copy = True
                    elif "[stop baking]" in line:
                        copy = False
                    elif copy:
                        bake_code += line + "\n"
                
                try:
                    exec(bake_code, globals(), self.data)
                except Exception as e:
                    raise ConfigError(f_loc, "[bake me]", str(e))
                
                f.close()
    
    def parse_deps(self, config):
        self.worker = workers[self.data["worker"]]
        # TODO check for legit action?
        for action in self.data["dependencies"]:
            self.deps[action] = set()
            for dep in self.data["dependencies"][action]:
                # format (regex): "unit>(worker)?>action"
                parts = dep.split(">")
                if len(parts) != 3:
                    raise ConfigError("dependency %s"%dep, "bake_me code for %s"*self.name)
                dname, dworker_s, daction = tuple(parts)
                dunit = create_unit(dname, config)
                dworker = workers[dworker_s]  # this can be None, but should be fixed by the Todo's
                # daction remains daction
                self.deps[action].add(Dependency(dunit, dworker, daction))
    
    def list_deps(self):
        print("\nUnit "+self.name)
        for action in self.deps:
            print("  - Action "+action)
            for dep in self.deps[action]:
                print("     - " + str(dep))
    
    def get_deps(self, action):
        if action in self.deps:
            return self.deps[action]
        else:
            return []
    
    def get_dep_actions(self):
        yield from self.deps
    
    def get_files(self):
        yield from self.files
    

class CollectionUnit(Unit):
    def __init__(self, name, config):
        assert name.startswith("@")
        self.name = name
        self.worker = None
        self.location = os.path.abspath(name[1:])
        self.subunits = []
        self.data = {}
        
        if name in config["collections"]:
            for subname in config["collections"][name]:
                self.subunits.append(create_unit(subname, config))
        else:
            for reld in os.listdir(self.location):
                d = os.path.abspath(os.path.join(self.location, reld))
                if os.path.isdir(d):
                    self.subunits.append(create_unit(os.path.relpath(d, os.getcwd()), config))
        
    def scan(self, config):
        # nothing to scan
        # TODO (hard): find a solution so you can add layers to the config for subunits
        pass
    
    def list_deps(self):
        print("\nCollection unit %s, containing units: %s"%(self.name, ", ".join([s.name for s in self.subunits])))
    
    def get_deps(self, action):
        deps = []
        for s in self.subunits:
            deps.append(Dependency(s, None, action))
        return deps
    
    def get_dep_actions(self):
        for i in self.subunits:
            yield from i.get_dep_actions()
    
    def get_files(self):
        for s in self.subunits:
            yield from s.get_files()
        
        
# -------------------------------------------------------------------------------
# Dependencies and Todo's

class Dependency:
    def __init__(self, unit, worker, action):
        self.unit = unit
        self.worker = pick_nonempty(worker, unit.worker)
        self.action = action
    
    def __str__(self):
        return self.unit.name + ">" + (str(self.worker.shortname) if self.worker is not None else "") + ">" + self.action
    
    __repr__ = __str__
    
    def __eq__(self, other):
        # not comparing self.deps, assuming that given the other 3, deps is already equal
        return (self.unit, self.worker, self.action) == (other.unit, other.worker, other.action)
    
    def __hash__(self):
        return hash((self.unit, self.worker, self.action))

NOT_DONE = 0   # do this.
UNCHANGED = 1  # dep not changed since previous run
JUST_DID = 2   # dep has changed since previous run, so redo parents

def done(status):
    return status != NOT_DONE

class Todo(Dependency):
    def __init__(self, dep):
        self.unit = dep.unit
        self.worker = dep.worker
        self.action = dep.action
        
        self.status = NOT_DONE
        
        # a Dependency may contain null values or empty string for worker/action...
        self.deps = set()
        init_deps = self.unit.get_deps(self.action)
        if len(init_deps) > 0:
            self.build_deps(init_deps)
        
        # also allow extra deps based on the worker and action
        extra_deps = self.worker.extra_deps(self)
        dbg_print("Getting extra deps for", self)
        dbg_print("     =", repr(extra_deps))
        self.build_deps(extra_deps)
        
    def new_action(self, new_action):
        newtodo = copy.copy(self)
        newtodo.action = new_action
        if newtodo in all_todos:
            return all_todos[newtodo]
        else:
            all_todos[newtodo] = newtodo
            return newtodo
    
    def build_deps(self, deps):
        for d in deps:
            self.deps.add(pick_nonempty_todo(d, self))
    
    def all_deps(self):
        yield self
        yield from self.all_deps_without_self()
    
    def all_deps_without_self(self):
        for d in self.deps:
            yield from d.all_deps()
    
    def list_deps(self, writer):
        writer.writeline("- " + str(self))
        writer.tab()
        for d in self.deps:
            d.list_deps(writer)
        writer.untab()
    
    def do(self, database, writer):
        writer.debugline("--> do " + str(self))
        # return True if we have actually performed work
        
        # check whether we already did this one before, this run of baker.py
        if done(self.status):
            writer.writeline(rgbtext("Already done '" + str(self)+"'", green))
            return self.status
        
        rebuild = False
        if len(self.deps) > 0:
            writer.writeline(rgbtext("Dependencies of Todo '" + str(self)+"':", cyan))
            writer.tab()
            for d in self.deps:
                writer.debugline("calling do " + str(d))
                status = d.do(database, writer)
                writer.debugline("sssstatus is " + str(status))
                # we need to rebuild when our deps have changed
                if status == JUST_DID:
                    assert(d.status == JUST_DID)
                    rebuild = True
                    writer.debugline("rebuild me pleeeeease")
            writer.untab()
        else:
            writer.debugline("no deps apparently")
        
        # check whether our unit is a collection
        if isinstance(self.unit, CollectionUnit):
            writer.writeline(rgbtext("No need to perform %s on the collection '%s'."%(self.action, self.unit.name), cyan))
            self.status = JUST_DID if rebuild else UNCHANGED
            return self.status
        
        if not rebuild:
            # check whether we did this one before, during some previous run of baker.py
            # basically, we have to redo this todo if and only if
            #    max(dates(input_files)) > min(dates(output_files_prev_run))
            input_files = self.worker.input_files(self)
            if self in database:
                writer.debugline("in database")
                output_files_prev_run = database.get_row(self)
                writer.debugline("input files: " + ", ".join(input_files))
                writer.debugline("output files prev run: " + ", ".join(output_files_prev_run))
                # no input files?
                max_input = max(dates_from_files(input_files)) if len(input_files) > 0 else MIN_DATE
                # no output files, but it is in the database? no need to redo
                min_output = min(dates_from_files(output_files_prev_run)) if len(output_files_prev_run) > 0 else MAX_DATE
                writer.debugline("dates max_input = {0}, min_output = {1}".format(max_input, min_output))
                if max_input > min_output:
                    return self.actually_do(writer, database)
                else:
                    writer.writeline(rgbtext("Already (previously) done '" + str(self) +"'", green))
                    self.status = UNCHANGED
                    return UNCHANGED
            else:
                # output files not in database: todo has never been executed before
                writer.debugline("not in database")
                return self.actually_do(writer, database)
        else:
            # deps rebuilt, we redo this one too
            writer.debugline("deps rebuilt, we rebuilt this one too")
            return self.actually_do(writer, database)
                
    
    def actually_do(self, writer, database):
        writer.writeline(rgbtext("Performing Todo '" + str(self)+"'", cyan))
        output = self.worker.do(self, writer)
        writer.debugline(("output = " + ", ".join(output)) if output is not None else "output = None")
        database.set_row(self, output)
        self.status = JUST_DID
        return JUST_DID


# See below
class FakeUnit(BaseUnit):
    def __init__(self, name, location, data):
        BaseUnit.__init__(self, name)
        self.location = location
        self.data = data

# Has no dependencies, and is mainly used to pass to a certain worker to get a something specific done
class FakeTodo:
    def __init__(self, unit_location, unit_data, worker, action):
        self.unit = FakeUnit("FAKE", unit_location, unit_data)
        self.worker = worker
        self.action = action
        self.status = NOT_DONE
    
    def build_deps():
        pass
        
    def all_deps():
        return
        yield
        
    def all_deps_without_self():
        return
        yield
    
    def list_deps():
        return
        yield


# creates only unique todos
def create_todo(dep):
    t = Todo(dep)
    if t in all_todos:
        return all_todos[t]
    all_todos[t] = t
    return t
    
    
def pick_nonempty_todo(high, low):  # ... priority
    # worker and action of low may be taken if high is empty for those attributes
    # high must always have a unit ofc
    return create_todo(Dependency(high.unit, pick_nonempty(high.worker, low.worker), pick_nonempty(high.action, low.action)))


class RootTodo(Todo):
    def __init__(self, worker, action, cmd_units):
        self.unit = BaseUnit(project_name)
        self.worker = worker
        self.action = action
        self.deps = set()
        self.build_deps([Dependency(u, None, "") for u in cmd_units])
    
    def do(self, database, writer):
        fails = []
        for d in self.deps:
            writer.indent = 0
            try:
                d.do(database, writer)
            except BuildError as e:
                writer.indent = 0
                writer.writeline(str(e))
                fails.append(d.unit.name)
            writer.writeline("")
        
        if len(fails) == 0:
            writer.writeline("All done!")
        else:
            writer.writeline("All done, but these units you specified failed: %s"%(", ".join(fails)))






# ---------------------------------------------------------------------------------------
# Workers

# needs_config has been extended!

# TODO more compiler/worker support may come
class Worker:
    shortname = "LAZY DEVELOPER DETECTED"  # aka override me pls
    needs_config = []
        
    def extra_deps(self, todo):
        # extra dependencies implicitly given by the action
        return set()  # by returns nothing
    
    def do(self, todo, writer):
        # by default does nothing, returns no output files
        # NOTE however, that if the worker actually did something but produced no output files,
        # you should return None instead of []
        return None
    
    def input_files(self, todo):
        return list(todo.unit.get_files())  # by default returns all files
                                            # always works, but less performant

class EasyWorker(Worker):
    def do(self, todo, writer):
        if todo.action in self.actions:
            return self.actions[todo.action](self, todo, writer)
        else:
            writer.writeline("Ignoring action %s on %s"%(todo.action,todo.unit.name))
            return []


# --- Actual workers ---

# A very configurable worker, on the level of the units themselves
class Custom(Worker):
    shortname = "custom"
    
    def do(self, todo, writer):
        if todo.action not in todo.unit.data:
            raise ConfigError("unit "+todo.unit.name, "function "+todo.action, "trying to use it as an action")
        return todo.unit.data[todo.action](todo, writer)
    

class GccCompiler(EasyWorker):
    shortname = "gcc"
    needs_config = ["gcc_config"]
    
    def __init__(self, config):
        self.__dict__.update(config)
        self.cmd_object = "{s.compiler} -{s.mode} -std={s.std} -c -x c++ {source} -o {objloc} {include} {s.extra}"
        self.cmd_exec = "{s.compiler} -{s.mode} -std={s.std} -o {execloc} {objects} {s.extra}"
    
    def extra_deps(self, todo):
        extra = set()
        if todo.action == "build_exec":
            extra.add(Dependency(todo.unit, todo.worker, "build_objects"))
            for d in todo.deps:
                dbg_print("exec has dep", d)
                if d.action == "build_objects":
                    for subd in d.all_deps():
                        if subd.action == "headers":
                            extra.add(Dependency(subd.unit, subd.worker, "build_objects"))
        
        elif todo.action == "headers":
            dbg_print("recursively adding headers")
            for d in todo.all_deps_without_self():
                dbg_print(" -> dep", d)
                if d.action == "headers":
                    extra.add(d)
            
        elif todo.action == "build_objects":
            extra.add(Dependency(todo.unit, todo.worker, "headers"))
            
            # difficult one: if a todo is to be turned into an executable, for every object it depends upon,
            # it must also depend on it's headers.
            if "build_exec" in todo.unit.get_dep_actions():
                for dep_obj in todo.unit.get_deps("build_exec"):
                    extra.add(Dependency(dep_obj.unit, todo.worker, "headers"))
            
        return extra
    
    
    def build_objects(self, todo, writer):
        output_files = []
        includes = self.get_dependent_includes(todo)
        
        # typical gcc call for objects:
        #  g++ -O3 -std=c++11 -I /include/me -c obj.cpp
        #  --> produces obj.o
        
        for source in glob.glob(os.path.join(todo.unit.location, "*.cpp")):
            filename = drop_ext(source)
            objloc = os.path.join(todo.unit.location, filename+".o")
            output_files.append(self.gcc_build_object(includes, source, objloc, writer, todo))
        
        return output_files
    
    
    def build_exec(self, todo, writer):
        # typical gcc call for executables:
        #  g++ -O3 -std=c++11 -I /include/me -o exec obj.o obj2.o obj3.o
        #  --> produces exec
        
        if "executable" in todo.unit.data:
            executable = todo.unit.data["executable"]
            objects = self.get_dependent_objects(todo)
            execloc = os.path.join(todo.unit.location, executable)
            return [ self.gcc_build_exec(execloc, objects, writer, todo) ]  # should be a list
        else:
            raise ConfigError(todo.unit.name, "executable", "name of the resulting executable")
        
    
    def get_dependent_objects(self, todo):
        objects = set(glob.glob(os.path.join(todo.unit.location, "*.o")))
        for d in todo.all_deps():
            if d.action == "build_objects":
                # dependencies should've already been built
                objects.update(glob.glob(os.path.join(d.unit.location, "*.o")))
        return objects
    
    
    def get_dependent_includes(self, todo):
        includes = set()
        for d in todo.all_deps():
            if d.action == "headers":
                includes.add(d.unit.location)
        return includes
    
    
    def gcc_build_exec(self, execloc, objects, writer, todo, extra=""):
        obj_str = " ".join([escape_path(o) for o in objects])
        check_call(self.cmd_exec.format(s=self, execloc=escape_path(execloc), objects=obj_str) +" "+extra, writer, todo)
        return execloc
    
    
    def gcc_build_object(self, includes, sourceloc, objloc, writer, todo, extra=""):
        includes.add(os.getcwd())
        include = " ".join(["-I %s/"%(escape_path(i)) for i in includes])
        check_call(self.cmd_object.format(s=self, source=escape_path(sourceloc), objloc=escape_path(objloc),
                                            include=include) +" "+extra, writer, todo)
        return objloc
    
    
    def headers(self, todo, writer):
        loc = os.path.join(todo.unit.location, ".headers_baked")
        f = open(loc, "w")
        f.close()
        return [loc]
    
    
    def find_headers(self, todo):
        files = []
        for ext in self.headers_globs:
            files.extend(glob.glob(os.path.join(todo.unit.location, ext)))
        return files
    
    actions = {
        "build_objects": build_objects,
        "build_exec": build_exec,
        "headers": headers,
    }


class Maintenance(EasyWorker):
    shortname = "maintenance"
    needs_config = ["maintenance_config"]
    
    def __init__(self, config):
        if "dirty_files" not in config:
            raise ConfigError("bake_project.py", "dirty_files", "a list of globs, eg. ['*.o', '~*']")
        self.__dict__.update(config)
    
    def clean(self, todo, writer):
        for regex in self.dirty_files:
            for l in glob.glob(os.path.join(todo.unit.location, regex)):
                os.remove(l)
        return None
    
    def clean_all(self, todo, writer):
        self.clean(todo, writer)
        exe = get("executable", todo.unit.data)
        if exe is not None:
            loc = os.path.join(todo.unit.location, exe)
            if os.path.isfile(loc):
                os.remove(loc)
        return None
    
    actions = {
        "clean": clean,
        "clean_all": clean_all  # also clean executables
    }


class Make(Worker):
    shortname = "make"
    
    def input_files(self, todo):
        l = super(Make, self).input_files(todo)
        l.append("Makefile")
        return l
    
    def do(self, todo, writer, cwd=None):
        if cwd is None:
            cwd = todo.unit.location
        # reminder: should return a list of output files!
        # unless it should always run, which is the case for Makefiles (as they have their own mechanisms for this)
        check_call("make "+todo.action, writer, todo, 
                   "Makefile may not contain your preferred action", cwd)
        # always run, return None
        return None


class CMake(Worker):
    shortname = "cmake"
    # only supports unix Makefiles, windows users can go f*** themselves
    
    def input_files(self, todo):
        if todo.action == "generate_makefile":
            l = super(CMake, self).input_files(todo)
            l.append(os.path.join(todo.unit.location, "CMakeLists.txt"))
            return l
    
    def extra_deps(self, todo):
        # every other action than generating the makefile kinda needs the makefile
        if todo.action != "generate_makefile":
            return set([Dependency(todo.unit, self, "generate_makefile")])
        return set()
    
    def do(self, todo, writer):
        cmake_stuff = os.path.join(todo.unit.location, "cmake_stuff")
        writer.debugline("cmake_stuff is " + cmake_stuff)
        if todo.action == "generate_makefile":
            if not os.path.exists(cmake_stuff):
                os.makedirs(cmake_stuff)
            check_call("cmake ..", writer, todo, cwd=cmake_stuff)
            # Vague problem with cmake #003051: when calling `cmake ..` like this, it does not touch all files.
            # This causes Baker to think it hasn't done any work at all, so it will keep redoing this.
            # However, if I run `cmake ..` myself (in zsh), all files are touched and Baker knows what's
            # going on. As a solution I manually touch each file.
            result = [os.path.join(todo.unit.location, "cmake_stuff", f) for f in os.listdir(cmake_stuff)]
            for f in result:
                touch(f)
            return result
        else:
            return Make.do(self, todo, writer, cmake_stuff)


# put a 't' in front of everything
# used internally in GTester, not for direct usage by user
class GccTestCompiler(GccCompiler):
    shortname = "gcc_gtest"
    
    def __init__(self, config, parent_gtest):
        self.parent_gtest = parent_gtest
        super(GccTestCompiler, self).__init__(config)
    
    def extra_deps(self, todo):
        extra = set()
        if todo.action == "build_test_exec":
            extra = GccCompiler.extra_deps(self, todo.new_action("build_exec"))
            extra.add(Dependency(todo.unit, self, "build_test_objects"))
        elif todo.action == "build_test_objects":
            extra = GccCompiler.extra_deps(self, todo.new_action("build_objects"))
            extra.add(Dependency(todo.unit, self.parent_gtest, "generate_tests"))
        return extra
    
    def build_test_exec(self, todo, writer):
        objects = self.get_dependent_objects(todo)
        objects.update(glob.glob(os.path.join(todo.unit.location, "*.to")))
        execloc = os.path.join(todo.unit.location, "test.bin")
        static_libs = escape_path(os.path.join(os.getcwd(), self.parent_gtest.static_libs))
        return [ self.gcc_build_exec(execloc, objects, writer, todo, "-pthread -L %s -lgtest -lgtest_main"%static_libs) ]
        
        
    def build_test_objects(self, todo, writer):
        output_files = []
        includes = self.get_dependent_includes(todo)
        includes.add(os.path.join(self.parent_gtest.location, "include"))  # should be gtest's location
        
        for source in glob.glob(os.path.join(todo.unit.location, "*.cpp")):
            filename = drop_ext(source)
            objloc = os.path.join(todo.unit.location, filename+".o")
            output_files.append(self.gcc_build_object(includes, source, objloc, writer, todo))
        
        for source in glob.glob(os.path.join(todo.unit.location, "*.tcpp")):
            filename = drop_ext(source)
            objloc = os.path.join(todo.unit.location, filename+".to")
            output_files.append(self.gcc_build_object(includes, source, objloc, writer, todo))
        
        return output_files
    
    
    actions = {
        "build_test_exec": build_test_exec,
        "build_test_objects": build_test_objects,
        "build_objects": GccCompiler.build_objects,
        "build_exec": GccCompiler.build_exec,
        "headers": GccCompiler.headers,
    }


class GTester(EasyWorker):
    shortname = "gtest"
    needs_config = ["gtest_config", "gcc_config"]
    
    def __init__(self, config, gcc_config):
        self.__dict__.update(config)
        #mod_gcc_config = gcc_config.new_extra_layer({
            #"default_executable": "tester",
            #"extra": "-lgtest -lgtest_main",
        #})
        self.gcc = GccTestCompiler(gcc_config, self)
    
    def extra_deps(self, todo):
        extra = set()
        if todo.action == "build_tests":
            extra.add(Dependency(todo.unit, self.gcc, "build_test_exec"))
        return extra
        
    
    def generate_tests(self, todo, writer):
        output_files = []
        
        # Step 0: where are all the .test files?
        test_files = glob.glob(os.path.join(todo.unit.location, "*.test"))
        content = ""
        for fname in test_files:
            tf = open(fname, "r")
            content += tf.read() + "\n"
            tf.close()
        
        writer.debugline("content = " + content)
        writer.debugline("template = " + self.template)
        
        # Step 1: generate a cpp containing all the tests
        gen_cpp = os.path.join(todo.unit.location, "tests.tcpp")
        output_files.append(gen_cpp)
        f = open(gen_cpp, "w")
        f.write(self.template.format(tests=content))
        f.close()
        
        return output_files
        
    def build_tests(self, todo, writer):
        writer.writeline("The test executable is waiting for you :)")

    actions = {
        "build_tests": build_tests,
        "generate_tests": generate_tests,
    }
    
# ------------------------------------------------------------------------------------
# Main and config stuff

global workers_list
workers_list = [Custom, GccCompiler, Maintenance, Make, CMake, GTester]

global workers
workers = {}

# if none is specified on cmd
standard_workers_by_action = {
    "build_exec": "gcc",
    "build_objects": "gcc",
    "clean": "maintenance",
    "clean_all": "maintenance",
    "build_tests": "gtest",
}
                    

global project_name
project_name = os.path.split(os.getcwd())[-1]
        
    
def main():
    # which unit to bui-- perform some action on?
    
    parser = argparse.ArgumentParser(description="A build tool that works.")
    parser.add_argument("action", metavar="A", type=str, help="The action the worker should perform")
    parser.add_argument("units", metavar="U", type=str, nargs="+", help="A unit (given by relative path) to use")
    # TODO other actions
    parser.add_argument("--worker", metavar="W", type=str, help="worker to use", choices=[w.shortname for w in workers_list], default="DEFAULT_BY_ACTION")
    parser.add_argument("--list-deps", action="store_true", help="simply list the dependencies of all units concerned", default=False)
    parser.add_argument("--debug", action="store_true", help="show extra debug info", default=False)
    parser.add_argument("--trace", action="store_true", help="python debugger set_trace()", default=False)
    
    args = parser.parse_args()
    
    writer = IndentWriter(args.debug)
    
    project_config = {}
    if not exec_file("bake_project.py", project_config):
        raise ConfigError("this directory", "bake_project.py", "an empty file works too")
    
    # NOTE this is different from the original Baker
    #default_config = {}
    #assert exec_file("default.py", default_config)
    config = HardConfig([project_config])
    
    db = Database(".baking_database")
    
    # parse workers
    worker_classes = {}
    for w in workers_list:
        worker_classes[w.shortname] = w
    
    for w in workers_list:
        workers[w.shortname] = parse_worker(w, config, worker_classes)
    
    workers[""] = None
    
    cmd_units = set()
    
    for uname in args.units:   
        u = create_unit(uname, config)
        cmd_units.add(u)

    
    if args.list_deps:
        for u in all_units.values():
            u.list_deps()
    else:
        # already checked for 'emptiness' in argparser
        default_worker = ""
        if args.worker == "DEFAULT_BY_ACTION":
            default_worker = workers[standard_workers_by_action[args.action]]
        else:
            default_worker = workers[args.worker]
        
        default_action = args.action
        
        root = RootTodo(default_worker, default_action, cmd_units)
        if (writer.debug):
            root.list_deps(writer)
        if (args.trace):
            pdb.set_trace()
        else:
            root.do(db, writer)
        
        db.write()
        
        

if __name__ == "__main__":
    try:
        main()
    except BakeError as e:
        print(e)
    
