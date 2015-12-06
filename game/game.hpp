
/* [bake me]

dependencies["headers"] = [
	"event>>headers",
	"libs/tinyxml>>headers",
	"view>>headers",
	"model>>headers",
	"util/ccq>>headers",
]

[stop baking] */

#pragma once

#include <memory>
#include <set>
#include <mutex>

#include "libs/picojson.hpp"

#include "event/event.hpp"
#include "view/view.hpp"
#include "controller/controller.hpp"
#include "model/model.hpp"
#include "util/ccq/ccq.hpp"

namespace si {

class Game {
public:
	Game(const picojson::value& conf);
	
	// --- starts everything! ---
	// register first
	void run();
	
	
	// --- [un]registering ---
	void registerView(view::View* v);
	void unregisterView(view::View* v);
	
	void registerController(controller::Controller* c);
	void unregisterController(controller::Controller* c);
	
	
	// --- communication between M/V/C ---
	// Model --> Views
	void notifyViews(Event* e);
	// Controllers --> Model
	void notifyModel(Event* e);
	
	// so the view can get info about the model if needed
	const model::Model& get_model() const;
	
	// gives an Event* to the model when asked, could be nullptr
	Event* get_controller_event();
	
	void model_lock();
	void model_unlock();
	
private:
	
	// the model, also contains a stopwatch
	model::Model the_model;
	// CCQ for the model
	util::CCQueue<Event*> model_queue;
	// mutex for the model
	std::mutex model_mutex;
	
	// controllers
	std::set<controller::Controller*> controllers;
	
	// views, each one has a CCQ
	std::map<view::View*, std::unique_ptr<util::CCQueue<Event*>>> views;
};

}
