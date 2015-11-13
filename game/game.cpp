
#include "game.hpp"

#include <cassert>
#include <thread>
#include <vector>

using namespace si;

Game::Game(const TiXmlDocument& doc) {
	the_model = model::Model(doc, this);
}

void Game::notifyModel(Event* e) {
	the_model.handleEvent(e);
}

void Game::notifyViews(Event* e) {
	for (auto& v: views) {
		v->handleEvent(e);
	}
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

const model::Model& Game::get_model() {
	return the_model;
}



// helper functions
void start_controller(controller::Controller* c) {
	c->start();
}

void start_view(view::View* v) {
	v->start();
}

void Game::run() {
	std::vector<std::thread> threads(views.size() + controllers.size());
	int i = 0;
	for (controller::Controller* c: controllers) {
		threads[i] = std::thread(start_controller, c);
		i++;
	}
	
	for (view::View* v: views) {
		threads[i] = std::thread(start_view, v);
		i++;
	}
	
	// TODO TICKS!
	// ... work is being done ...
	// ... lots of Aliens are being murdered ...
	
	for (auto& t: threads) {
		// join on all views/controllers
		t.join();
	}
}

