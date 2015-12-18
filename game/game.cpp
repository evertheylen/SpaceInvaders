
#include "game.hpp"

#include <cassert>
#include <thread>
#include <set>
#include <memory>
#include <utility>

using namespace si;

Game::Game(const picojson::value& conf) {
	the_model = model::Model(conf, this);
}

void Game::notifyControllers(Event* e) {
	for (controller::Controller* c: controllers) {
		c->handleEvent(e);
	}
}


void Game::notifyViews(Event* e) {
	for (view::View* v: views) {
		if (v->isConcurrent()) {
			assert(dynamic_cast<Tick*>(e) == nullptr);
			v->handleEvent(e->clone());
		} else {
			v->handleEvent(e->clone());
		}
	}
	delete e;
}

void Game::notifyViews(Tick* e) {
	for (view::View* v: synchr_views) {
		v->handleEvent(e->clone());
	}
	delete e;
}

void Game::notifyModel(Event* e) {
	model_queue.push(e);
}


void Game::registerView(view::View* v) {
	views.insert(v);
	if (not v->isConcurrent()) {
		synchr_views.insert(v);
	}
}

void Game::unregisterView(view::View* v) {
	views.erase(v);
}

void Game::registerController(controller::Controller* c) {
	controllers.insert(c);
}

void Game::unregisterController(controller::Controller* c) {
	controllers.erase(c);
}

const model::Model& Game::get_model() const {
	return the_model;
}

int Game::get_player() {
	model_lock();
	for (unsigned int i: the_model.leftover_players) {
		the_model.leftover_players.erase(i);
		model_unlock();
		return i;
	}
	model_unlock();
	return -1;
}


void Game::run() {
	std::set<std::thread*> threads;
	
	// starting happens synchronously!
	for (view::View* v: views) {
		threads += v->start();
	}
	
	for (controller::Controller* c: controllers) {
		threads += c->start();
	}
	
	// Now that all views / controllers are running, give them an extra Init event
	// to initialize whatever they want...
	notifyViews(new Init);
	notifyControllers(new Init);
	
	std::set<std::thread*> model_threads;
	model_threads += the_model.start();
	// ... work is being done ...
	// ... lots of Aliens are being murdered ...
	
	for (std::thread* t: threads) {
		// join on all threads
		if (t != nullptr) {
			t->join();
			delete t;
		}
	}
	
	std::cout << "Waiting for model to stop...\n";
	
	// all views / controllers are done
	// signal to the model it should quit
	notifyModel(new si::VCStop);
	for (std::thread* t: model_threads) {
		if (t != nullptr) {
			t->join();
			delete t;
		}
	}
	
}


Event* Game::get_controller_event() {
	if (not model_queue.empty()) {
		return model_queue.pop();
	} else {
		return nullptr;
	}
}


void Game::model_lock() {
	model_mutex.lock();
}

void Game::model_unlock() {
	model_mutex.unlock();
}


