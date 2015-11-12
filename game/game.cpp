
#include "game.hpp"

#include <cassert>
#include <thread>
#include <vector>

using namespace si;

Game::Game(const TiXmlDocument& doc) {
	m_model = std::unique_ptr<model::Model>(new model::Model(doc, this));
}

void Game::notifyModel(Event* e) {
	// only notify model if running
}

void Game::notifyViews(Event* e) {
	// only notify views if running
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
	
	// work is being done
	i = 0;
	for (auto& t: threads) {
		t.join();
	}
}

