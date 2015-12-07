
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

void Game::notifyViews(Event* e) {
	for (auto& v: views) {
		v.second->push(e->clone());
		v.first->wake_up();
	}
	delete e;
}

void Game::notifyModel(Event* e) {
	model_queue.push(e);
}


void Game::registerView(view::View* v) {
	views[v] = std::unique_ptr<util::CCQueue<Event*>>(new util::CCQueue<Event*>());
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
	
	for (auto& it: views) {
		threads += it.first->start();
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

Event* Game::get_view_event(view::View* v) {
	util::CCQueue<Event*>* q = views[v].get();
	if (not q->empty()) {
		return q->pop();
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


