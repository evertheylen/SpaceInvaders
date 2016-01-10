
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
#include "util/rwlock/rwlock.hpp"

namespace si {

class Game {
public:
	Game(const picojson::value& conf);
	
	// --- starts everything! ---
	// register first
	void run();
	
	
	// --- [un]registering ---
	void register_view(view::View* v);
	void unregister_view(view::View* v);
	
	void register_controller(controller::Controller* c);
	void unregister_controller(controller::Controller* c);
	
	
	// --- communication between M/V/C ---
	// The model pushes messages to both views (for nearly all output) and controllers
	// (for important state changes, eg. "the game is over stop processing input please").
	void notify_all(Event* e); // for important state changes
	void notify_controllers(Event* e);
	void notify_views(Event* e);
	void notify_views(Tick* e); // simple optimization (Tick is subclass of Event)
	// The model pulls input from the controller (nullptr if no input)
	Event* get_input_event();
	
	// get (const!) info about the model if needed
	const model::Model& get_model() const;
	
	
	// --- avoiding race conditions ---
	void model_lock();
	void model_unlock();
	
	// RWLock for the entities (TODO private?)
	util::RWLock entity_lock;
	
	
	// sadly, a 'hack' around pthread's bugs
	bool has_cc_viewers() { return synchr_views.size() < views.size(); }
	
private:
	
	// the model, also contains a stopwatch
	model::Model the_model;
	
	// mutex for the model
	std::mutex model_mutex;
	
	util::CCQueue<Event*> model_input;
	
	// controllers
	std::set<controller::Controller*> controllers;
	
	// views, each one *could* have a CCQ
	std::set<view::View*> views;
	std::set<view::View*> synchr_views; // subset containing synchronous views
};

}
