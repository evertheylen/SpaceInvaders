
/* [bake me]

dependencies["headers"] = [
	"event>>headers",
	"libs/tinyxml>>headers",
]

[stop baking] */

#pragma once

#include "libs/tinyxml/tinyxml.h"
#include "event/event.hpp"

namespace si {

class Game {
public:
	Game(const TiXmlDocument& doc);
	
	void notifyViews(Event* e);
	
	void notifyModels(Event* e);
	
	void registerView(Event* e);
	
	void run();
	// starts the stopwatch and enables communication between MVC

private:
	// stopwatch
	// the model
	// controllers
	// views
};

}
