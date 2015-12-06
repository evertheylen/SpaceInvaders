
#include "game.hpp"

#include <cassert>
#include <thread>
#include <vector>
#include <memory>
#include <utility>

using namespace si;

Game::Game(const picojson::value& conf) {
	the_model = model::Model(conf, this);
}

void Game::notifyViews(Event* e) {
	for (auto& v: views) {
		// TODO put in CCQ
		v.first->handleEvent(e);
	}
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
	std::vector<std::unique_ptr<std::thread>> threads(views.size() + controllers.size() + 1);
	int i = 0;
	// starting happens synchronously!
	for (controller::Controller* c: controllers) {
		threads[i] = std::unique_ptr<std::thread>(c->start());
		i++;
	}
	
	for (auto& it: views) {
		threads[i] = std::unique_ptr<std::thread>(it.first->start());
		i++;
	}
	
	// ... work is being done ...
	// ... lots of Aliens are being murdered ...
	threads[i] = std::unique_ptr<std::thread>(the_model.start());
	
	for (auto& t: threads) {
		// join on all views/controllers
		t->join();
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


