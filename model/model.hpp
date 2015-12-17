
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
// TODO EntityIterator not exposing unique_ptr?
class EntityRange {
public:
	using CollectionT = std::vector<std::unique_ptr<Entity>>;
	
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
	
	// gives a player pointer to a controller
	Player* get_player() const;
	
	// iterate over all entities
	EntityRange all_entities() const;
	friend class EntityRange;
	
	// make sure all the handlers can actually access this class
	template <typename T>
	friend class _handleEvent_specialization;
	
private:
	void handleEvent(Event* e);
	
	// Actual gameplay stuff
	unsigned int max_players;
	std::vector<Level> levels;
	std::vector<std::unique_ptr<Entity>> entities;
	std::vector<Player*> players;
	
	// Important data about the game
	// May be accessed by anyone
	std::atomic<std::set<unsigned int>> leftover_players;
	
	Game* game;
	util::Stopwatch watch;
};

}

}

