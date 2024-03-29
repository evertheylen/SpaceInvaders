
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
#include "util/random/random.hpp"

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
	double alien_speed;
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
	const State& get_state() const { return state; }
	Player* get_player(int ID) const;
	
private:
	void handle_event(Event* e);
	
	void load_level(Level& l);
	void unload_level();
	void place_player(Level& l, int ID, Player* p);
	
	void check_collisions(Entity* e);
	bool check_collision(Entity* a, Entity* b);
	
	void move_alien(int& row, int& col);
	bool next_alien(int& row, int& col);
	
	void win_level();
	void lose_level();
	
	// === Actual gameplay data ===
	unsigned int max_players;
	std::vector<Level> levels;
	int current_level = -1;
	Entity world; // spans the entire world
	std::set<int> leftover_players;
	int players_alive;
	
	util::Periodical alien_periodical; // when to move an alien (not using movement vector, move in steps)
	enum alien_mov_state_type { LEFT, RIGHT, DOWNLEFT, DOWNRIGHT };
	alien_mov_state_type alien_mov_state = RIGHT;
	unsigned int moving_alien_x = 0;
	unsigned int moving_alien_y = 0;
	int aliens_alive;
	std::vector<std::vector<Alien*>> alien_grid; // alien_grid[col][row]  (like (x,y) coords, may contain nullptrs)
	Alien* topleftmost = nullptr;
	Alien* toprightmost = nullptr;
	std::vector<Alien*> bottom_aliens; // may contain nullptr's, index is column
		// because of the inverted y axis, this means the aliens here have the highest y of their column
	
	std::set<Entity*> entities;
	std::set<Entity*> saved_entities;  // entities saved here
	std::map<unsigned int, Player*> players;  // and here 
	
	// ======
	
	void playing();
	void wait();
	void recap();
	void gameover();
	State state;
	
	bool victory;
	
	Game* game;
	util::Stopwatch watch;
	
	// Statistics
	long int ticks = 0;
	double avg_tick = 0.0;
};

}

}

