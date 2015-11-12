
/* [bake me]

dependencies["headers"] = [
	"event>>headers",
	"libs/tinyxml>>headers",
	"view>>headers",
	"model>>headers",
]

[stop baking] */

#pragma once

#include <memory>
#include <set>

#include "libs/tinyxml/tinyxml.h"

#include "event/event.hpp"
#include "view/view.hpp"
#include "controller/controller.hpp"
#include "model/model.hpp"

namespace si {

class Game {
public:
	Game(const TiXmlDocument& doc);
	
	void notifyViews(Event* e);
	void notifyModel(Event* e);
	
	void registerView(view::View* v);
	void unregisterView(view::View* v);
	
	void registerController(controller::Controller* c);
	void unregisterController(controller::Controller* c);
	
	void run();
	// starts the stopwatch and enables communication between MVC

private:
	// stopwatch
	
	// the model
	std::unique_ptr<model::Model> m_model;
	
	// controllers
	std::set<controller::Controller*> controllers;
	
	// views
	std::set<view::View*> views;
};

}
