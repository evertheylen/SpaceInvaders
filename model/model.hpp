
/* [bake me]

dependencies["headers"] = [
	"libs/tinyxml>>headers",
	"event>>headers",
	"model/entity>>headers",
	"util/stopwatch>>headers",
]

dependencies["build_objects"] = [
	"game>>headers",
	"util>>headers"
]

[stop baking] */

#pragma once

#include <set>
#include <memory>
#include <string>
#include <thread>

#include "libs/picojson.hpp"

#include "event/event.hpp"
#include "model/entity/entity.hpp"
#include "util/stopwatch/stopwatch.hpp"

namespace si {

class Game;

namespace model {

// A level. Basically a POD class with a fancy constructor
class Level {
public:
	Level() = default;
	Level(const picojson::value& conf);
	
	unsigned int players;
	bool aliens; // TODO :P
	std::string name;
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
	
	std::thread* start();
	
	// gives a player pointer to a controller
	Player* get_player() const;
	
	// Actual game state:
	// TODO private
	std::set<std::unique_ptr<Entity>> entities;
	
private:
	Player* player;
	Game* game;
	
	util::Stopwatch watch;
};

}

}
