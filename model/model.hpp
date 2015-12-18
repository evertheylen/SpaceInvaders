
/* [bake me]

dependencies["headers"] = [
	"libs/tinyxml>>headers",
	"event>>headers",
	"model/entity>>headers",
	"util/stopwatch>>headers",
	"util/parser>>headers",
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

#include "model_state.hpp"

namespace si {

class Game;

namespace model {

class Model;

// Multimethods stuff
using yorel::multi_methods::virtual_;
MULTI_METHOD(_handleEvent, void, si::model::Model*, virtual_<si::Event>&);

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



// The model.
class Model {
public:
	Model() = default;
	
	Model(const picojson::value& conf, Game* g);
	
	// loop() does two things:
	//   - check for Events on the CCQ
	//   - tick (using the stopwatch)
	void loop();
	
	std::vector<std::thread*> start();
	
	// iterate over all entities
	EntityRange all_entities() const;
	friend class EntityRange;
	
	// make sure all the handlers can actually access this class
	template <typename T>
	friend class _handleEvent_specialization;
	
	// Important data about the game
	// May be accessed by anyone
	std::set<unsigned int> leftover_players;
	
	State get_state() const { return state; }
	
private:
	void handleEvent(Event* e);
	
	void loadLevel(Level& l);
	void unloadLevel();
	
	// Actual gameplay stuff
	unsigned int max_players;
	std::vector<Level> levels;
	int current_level = -1;
	std::set<Entity*> entities;
	std::set<Entity*> saved_entities;
	std::map<unsigned int, std::unique_ptr<Player>> players;
	bool victory = false;
	
	void Playing();
	void Wait();
	void Recap();
	State state = WAIT;
	
	Game* game;
	util::Stopwatch watch;
};

}

}

