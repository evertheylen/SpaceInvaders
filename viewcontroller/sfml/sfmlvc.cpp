
#include "sfmlvc.hpp"

#include <iostream>

#include "controller/sfml/sfmlcontroller.hpp"
#include "view/sfml/sfmlview.hpp"

using namespace si::vc;


void SfmlBase::start() {
	assert(handle != nullptr);
	handle->start();
}



SfmlVc::SfmlVc(unsigned int width, unsigned int height):
		window(sf::VideoMode(width, height), "Space Invaders") {
	// drawing is done from another thread, not this one, so disable it
	window.setActive(false);
}


void SfmlVc::start() {
	if (running) return;
	running = true;
	
	std::cout << "SfmlVc Started\n";
	
	if (controller != nullptr) {
		std::cout << "SfmlVc Input loop started\n";
		sf::Event event;
		while (window.waitEvent(event)) {
			// SFML likes blocking so we do too
			controller->handleSfmlEvent(event);
			if (event.type == sf::Event::Closed) {
				std::cout << "SfmlVc got close Event\n";
				window.close();
				break;
			}
		}
	}
	running = false;
	
	std::cout << "SfmlVc Stopped\n";
}

void SfmlVc::couple_controller(si::controller::SfmlController* _controller) {
	// friends!
	_controller->handle = this;
	controller = _controller;
}

void SfmlVc::decouple_controller() {
	controller = nullptr;
}

void SfmlVc::couple_view(si::view::SfmlView* _view) {
	// friends!
	_view->handle = this;
	view = _view;
}

void SfmlVc::decouple_view() {
	view->handle = nullptr;
}


