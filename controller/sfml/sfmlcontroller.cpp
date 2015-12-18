
#include <vector>

#include "sfmlcontroller.hpp"

#include "view/sfml/sfmlview.hpp"
#include "game/game.hpp"
#include "util/util.hpp"

using namespace si::controller;
using namespace si;

SfmlController::SfmlController(Game* g):
		game(g) {}


std::vector<std::thread*> SfmlController::start() {
	std::cout << "Controller has started\n";
	std::vector<std::thread*> v = {new std::thread(&SfmlController::loop, this)};
	return v + SfmlBase::start();
}

void SfmlController::handleEvent(Event* e) {
	// let the other thread handle it
	queue.push(e);
	slp.wake_up();
}


void SfmlController::loop() {
	while (true) {
		while (not queue.empty()) {
			std::cout << "SfmlController: got some event (in loop)\n";
			Event* e = queue.pop();
			_handleEvent(this, *e);
			if (state == model::EXIT) return; // thereby closing this Controller
		}
		slp.sleep();
	}
}

