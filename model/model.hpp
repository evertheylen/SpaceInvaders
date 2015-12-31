
/* [bake me]

dependencies["headers"] = [
	"libs/tinyxml>>headers",
	"event>>headers",
	"model/entity>>headers",
	"util/stopwatch>>headers",
	"util/parser>>headers",
	"util/periodical>>headers",
]

dependencies["build_objects"] = [
	"game>>headers",
	"util>>headers"
]

[stop baking] */

#pragma once

#include <vector>
#include <memory>
#include <string>
#include <thread>
#include <set>
#include <atomic>

#include "libs/picojson.hpp"
#include "yorel/multi_methods.hpp"

#include "event/event.hpp"
#include "model/entity/entity.hpp"
#include "util/stopwatch/stopwatch.hpp"
#include "util/parser/parser.hpp"
#include "util/periodical/periodical.hpp"

#include "model_state.hpp"

namespace si {

class Game;

namespace model {

class Model;


// A level. Basically a POD class with a fancy constructor
class Level {
public:
	Level() = default;
	Level(const picojson::object& conf);
	
	std::string name;
	unsigned int width;
	unsigned int height;
	unsigned int alien_rows;
	unsigned int alien_cols;
};


// for use in a range-based for loop
class EntityRange {
public:
	using CollectionT = std::set<Entity*>;
	
	EntityRange(const Model& _model);
	
	typename CollectionT::const_iterator begin();
	
	typename CollectionT::const_iterator end();
	
	unsigned int size();
	
private:
	const Model& model;
};


// Multimethods stuff
using yorel::multi_methods::virtual_;
MULTI_METHOD(_handle_event, void, si::model::Model*, const virtual_<si::Event>&);

MULTI_METHOD(_collide, void, si::model::Model*, virtual_<Entity>&, virtual_<Entity>&);

MULTI_METHOD(_kill, bool, si::model::Model*, virtual_<Entity>&);

// The model.
class Model {
public:
	Model() = default;
	
	Model(const picojson::value& conf, Game* g);
	
	// loop() does two things:
	//   - check for Events from controllers or the game
	//   - tick (using the stopwatch)
	void loop();
	
	std::vector<std::thread*> start();
	
	// Generators are one of my favorite things in Python, too bad we don't have that in C++!
	// Generators would mean I could organize my data the way I wanted (not a bunch of Entity* 
	// pointers), and when a View needs to iterate over all of them, I could write a simple
	// function that simply yields all the Entity pointers. The compiler may even be able
	// to 'unroll' the loop using the generator, which would be a huge benefit.
	// Instead, we have lousy iterators. This iterates over all entities.
	EntityRange all_entities() const;
	friend class EntityRange;
	
	// make sure all the handlers can actually access this class
	template <typename T>
	friend class _handle_event_specialization;
	
	template <typename T>
	friend class _collide_specialization;
	
	template <typename T>
	friend class _kill_specialization;
	
	// Important data about the game
	// May be accessed by anyone
	std::set<unsigned int> leftover_players;
	
	State get_state() const { return state; }
	bool game_won() const { return victory; }
	Level* get_current_level();
	
private:
	void handle_event(Event* e);
	
	void load_level(Level& l);
	void unload_level();
	
	void check_collisions(Entity* e);
	bool check_collision(Entity* a, Entity* b);
	
	// === Actual gameplay data ===
	unsigned int max_players;
	std::vector<Level> levels;
	int current_level = -1;
	Entity world; // spans the entire world
	
	util::Periodical alien_periodical; // when to move (not using movement vector, move in steps)
	std::vector<std::vector<Alien*>> alien_grid; // alien_grid[col][row]  (like (x,y) coords, may contain nullptrs)
	Alien* topleftmost = nullptr;
	Alien* toprightmost = nullptr;
	std::vector<Alien*> bottom_aliens; // may contain nullptr's, index is column
		// because of the inverted y axis, this means the aliens here have the highest y of their column
	
	std::set<Entity*> entities;
	std::set<Entity*> saved_entities;  // entities saved here
	std::map<unsigned int, Player*> players;  // and here 
	bool victory = false;
	
	// ======
	
	void playing();
	void wait();
	void recap();
	State state = WAIT;
	
	Game* game;
	util::Stopwatch watch;
	
	// Statistics
	long int ticks = 0;
	double avg_tick = 0.0;
};

}

}

