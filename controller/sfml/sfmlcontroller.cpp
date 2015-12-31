
#include <vector>

#include "sfmlcontroller.hpp"

#include "view/sfml/sfmlview.hpp"
#include "game/game.hpp"
#include "util/util.hpp"

using namespace si::controller;
using namespace si;

SfmlController::SfmlController(Game* g, bool _concurrent):
		game(g), concurrent(_concurrent) {}


std::vector<std::thread*> SfmlController::start() {
	assert(handle != nullptr);
	std::cout << "Controller has started\n";
	if (concurrent) {
		std::vector<std::thread*> v = {new std::thread(&SfmlController::loop, this)};
		return v + handle->start();
	} else {
		handle->init();
		return {};
	}
}

void SfmlController::handle_event(Event* e) {
	std::cerr << " [ V   M  >C ] SfmlController received: " << e->name() << "\n";
	if (concurrent) {
		// let the other thread handle it
		input_queue.push(e);
		slp.wake_up();
	} else {
		if (state != model::EXIT) _handle_event(this, *e);;
	}
}

Event* SfmlController::get_event() {
	if (not output_queue.empty()) {
		return output_queue.pop();
	}
	if (not concurrent) {
		sf::Event se;
		if (handle->window->pollEvent(se)) {
			// handle this event as if it was input
			if (se.type == sf::Event::Closed) {
				std::cout << "SfmlVc got close Event\n";
				handle_event(new si::SfmlExit);
			} else {
				handle_event(new si::SfmlInput(se));
			}
			// resulting output should be on the output_queue
			if (not output_queue.empty()) {
				return output_queue.pop();
			}
		}
	}
	return nullptr;
}

void SfmlController::loop() {
	while (true) {
		while (not input_queue.empty()) {
// 			std::cout << "SfmlController: got some event (in loop)\n";
			Event* e = input_queue.pop();
			_handle_event(this, *e);
			if (state == model::EXIT) return; // thereby closing this Controller
		}
		slp.sleep();
	}
}

