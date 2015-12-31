
#include "model.hpp"
#include "game/game.hpp"
#include "exceptions/exceptions.hpp"


using namespace si;
using namespace si::model;
using namespace si::util;

// Level
Level::Level(const picojson::object& conf) {
	name = get<std::string>(conf, "name");
	width = get<unsigned int>(conf, "width");
	height = get<unsigned int>(conf, "height");
	alien_rows = get<unsigned int>(conf, "alien_rows");
	alien_cols = get<unsigned int>(conf, "alien_cols");
}



// EntityRange

EntityRange::EntityRange(const Model& _model):
		model(_model) {}

typename EntityRange::CollectionT::const_iterator EntityRange::begin() {
	return model.entities.begin();
}

typename EntityRange::CollectionT::const_iterator EntityRange::end() {
	return model.entities.end();
}

unsigned int EntityRange::size() {
	return model.entities.size();
}



// Model

Model::Model(const picojson::value& conf, Game* g): game(g) {
	if (conf.is<picojson::object>()) {
		picojson::object m = conf.get<picojson::object>();
		max_players = get<unsigned int>(m, "max_players");
		if (m.find("levels")->second.is<picojson::array>()) {
			for (const auto& l: m.find("levels")->second.get<picojson::array>()) {
				levels.push_back(Level(convert<picojson::object>(l)));
			}
		} else {
			throw ParseError("levels should be an array");
		}
	} else {
		throw ParseError("Root should be an object");
	}
	
	for (unsigned int i=0; i<max_players; i++) {
		leftover_players.insert(i);
	}
}

std::vector<std::thread*> Model::start() {
	// TODO assert only one thread
	return {new std::thread(&Model::loop, this)};
}

void Model::loop() {
	// The game has started!
	// It's important I notify the controllers first
	// The controller needs to start the window
	game->notify_controllers(new ModelStart);
	game->notify_views(new ModelStart);
	
	while (true) {
		switch (state) {
			case State::WAIT:
				wait();
				break;
			case State::PLAYING:
				playing();
				break;
			case State::RECAP:
			case State::GAMEOVER:
				recap();
				break;
			case State::EXIT:
				return;
		}
	}
}

void Model::playing() {
	bool wait = game->has_cc_viewers();
	util::Stopwatch::TimePoint current_tick = watch.now();
	util::Stopwatch::TimePoint prev_tick;
	
	while (state == PLAYING) {
		// tick
		prev_tick = current_tick;
		current_tick = watch.now();
		util::Stopwatch::Duration duration = current_tick - prev_tick;
		
		// statistics
		if (ticks%100001 == 0) {
			std::cout << "Model: Did 100000 ticks, avg: " << avg_tick << "\n";
			ticks = 1;
			avg_tick = duration.count();
		} else {
			avg_tick += (duration.count() - avg_tick)/double(ticks);
			ticks++;
		}
		
		// update views
		// Could be concurrent, could be blocking. The View is responsible for that.
		game->notify_views(new Tick);
		
		game->entity_lock.write_lock();
		
		// --- Actual calculations of next 'frame' ---
		for (Entity* e: entities) {
			if (e->mov.dir.length() > 0) {
				e->mov.perform(duration, e->pos);
				check_collisions(e);
				if (not check_collision(e, &world)) {
					e->killme = true; // out of world
				}
			}
		}
		
		// handle Events, until nullptr
		while (Event* e = game->get_input_event()) {
			handle_event(e);
		}
		
		// delete entities
		// Again, this would be easier with generators
		for (auto iter = saved_entities.begin(); iter != saved_entities.end(); ) {
			if ((*iter)->killme) {
				if (_kill(this, *(*iter))) {
					delete *iter;
					entities.erase(*iter);
					saved_entities.erase(iter++);
				}
			} else {
				++iter;
			}
		}
		
		for (auto iter = players.begin(); iter != players.end(); ) {
			if (iter->second->killme) {
				if (_kill(this, *(iter->second))) {
					entities.erase(iter->second);
					delete iter->second;
					players.erase(iter++);
				}
			} else {
				++iter;
			}
		}
		
		game->entity_lock.write_unlock();
		
		// BUG this is pthread's fault. While there are readers waiting, it does not give them a chance,
		// although it is set to PREFER_READER_NP
		if (wait) {
			std::this_thread::sleep_for(std::chrono::nanoseconds(5000));
		}
	}
}

void Model::wait() {
	while (Event* e = game->get_input_event()) {
		std::cerr << " [ V   M<--C ] Model pulling Event from Controllers: " << e->name() << "\n";
		handle_event(e);
	}
}

void Model::recap() {
	
}

// note: assumes a is the smaller object
bool Model::check_collision(Entity* a, Entity* b) {
	double xmin = b->pos.x;
	double xmax = b->pos.x + b->size.x;
	double ymin = b->pos.y;
	double ymax = b->pos.y + b->size.y;
	// is there a point of a (corners) within b?
	#define check_in_b(point_x, point_y) if ((xmin <= point_x and point_x <= xmax) and (ymin <= point_y and point_y <= ymax)) return true
	check_in_b(a->pos.x, a->pos.y);
	check_in_b(a->pos.x + a->size.x, a->pos.y);
	check_in_b(a->pos.x, a->pos.y + a->size.y);
	check_in_b(a->pos.x + a->size.x, a->pos.y + a->size.y);
	return false;
	#undef check_in_b
}


void Model::check_collisions(Entity* a) {
	for (Entity* b: entities) {
		if (a != b) {
			if (check_collision(a, b) or check_collision(b, a)) {
				_collide(this, *a, *b);
			}
		}
	}
}

Level* Model::get_current_level() {
	if (0 <= current_level and current_level < levels.size()) {
		return &levels[current_level];
	} else {
		return nullptr;
	}
}


void Model::load_level(Level& l) {
	unload_level();
	// set world entity for out-of-bounds checking
	world = Entity(0,0);
	world.size.x = l.width;
	world.size.y = l.height;
	
	// add players
	for (auto& it: players) {
		entities.insert(it.second);
	}
	
	// create aliens
	bottom_aliens.assign(l.alien_cols, nullptr);
	alien_grid.assign(l.alien_cols, {});
	for (auto& row: alien_grid) row.assign(l.alien_rows, nullptr);
	
	double x = 50;
	double add_to_x = 75;
	double add_to_y = 60;
	for (int col=0; col<l.alien_cols; col++) {
		double y = 100;
		for (int row=0; row<l.alien_rows; row++) {
			Alien* a = new Alien(x, y, col, row);
			alien_grid[col][row] = a;
			saved_entities.insert(a);
			entities.insert(a);
			y += add_to_y;
		}
		bottom_aliens.at(col) = alien_grid[col][l.alien_rows-1];
		x += add_to_x;
	}
	topleftmost = alien_grid[0][0];
	toprightmost = alien_grid[l.alien_cols-1][0];
	
	for (auto& e: saved_entities) {
		entities.insert(e);
	}
}


void Model::unload_level() {
	// not players
	for (Entity* e: saved_entities) {
		delete e;
	}
	alien_grid.clear();
	bottom_aliens.clear();
	topleftmost = nullptr;
	toprightmost = nullptr;
	saved_entities.clear();
	entities.clear();
}


EntityRange Model::all_entities() const {
	return EntityRange(*this);
}



