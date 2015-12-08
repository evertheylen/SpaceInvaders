
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

// TODO specialization for Tick*
void Game::notifyViews(Event* e) {
	for (view::View* v: views) {
		if (v->isConcurrent()) {
			// do not broadcast ticks
			if (dynamic_cast<Tick*>(e) != nullptr) {
				v->handleEvent(e->clone());
			}
		} else {
			v->handleEvent(e->clone());
		}
	}
	delete e;
}

void Game::notifyModel(Event* e) {
	model_queue.push(e);
}


void Game::registerView(view::View* v) {
	views.insert(v);
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



void Game::run() {
	std::set<std::thread*> threads;
	
	// starting happens synchronously!
	for (controller::Controller* c: controllers) {
		threads += c->start();
	}
	
	for (view::View* v: views) {
		threads += v->start();
	}
	
	// ... work is being done ...
	// ... lots of Aliens are being murdered ...
	threads += the_model.start();
	
	for (std::thread* t: threads) {
		// join on all threads
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


