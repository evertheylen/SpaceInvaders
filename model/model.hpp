
/* [bake me]

dependencies["headers"] = [
	"libs/tinyxml>>headers",
	"event>>headers",
	"model/entity>>headers",
]

dependencies["build_objects"] = [
	"game>>headers",
	"util>>headers"
]

[stop baking] */

#pragma once

#include <set>
#include <memory>

#include "libs/tinyxml/tinyxml.h"

#include "event/event.hpp"
#include "model/entity/entity.hpp"

namespace si {

class Game;

namespace model {

class Model {
public:
	Model() = default;
	
	Model(const TiXmlDocument& doc, Game* g);
	
	void handleEvent(si::Event* e);
	
	Player* get_player() const;
	
	// Actual game state:
	// TODO private
	std::set<std::unique_ptr<Entity>> entities;
private:
	Player* player;
	Game* game;
};

}

}
