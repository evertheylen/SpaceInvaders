
/* [bake me]

dependencies["headers"] = [
	"libs/tinyxml>>headers",
	"event>>headers"
]

dependencies["build_objects"] = [
	"game>>headers"
]

[stop baking] */

#pragma once

#include "libs/tinyxml/tinyxml.h"

#include "event/event.hpp"

namespace si {

class Game;

namespace model {

class Model {
public:
	Model(const TiXmlDocument& doc, Game* g);
	
	void handleEvent(si::Event* e);
	
private:
	Game* game;
};

}

}
