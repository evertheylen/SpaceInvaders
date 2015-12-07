
#include "sfmlvc.hpp"

#include <iostream>

#include "controller/sfml/sfmlcontroller.hpp"
#include "view/sfml/sfmlview.hpp"

using namespace si::vc;


std::vector<std::thread*> SfmlBase::start() {
	assert(handle != nullptr);
	return handle->start();
}


SfmlVc::SfmlVc(unsigned int width, unsigned int height):
		window(sf::VideoMode(width, height), "Space Invaders") {
	// drawing is done from another thread, not this one, so disable it
	window.setActive(false);
}

void SfmlVc::loop() {
	sf::Context context;
	if (controller != nullptr) {
		std::cout << "SfmlVc Input loop started\n";
		sf::Event event;

		// TODO segfault?
		while (window.isOpen() and window.waitEvent(event)) {
			// SFML likes blocking so we do too
			std::cout << "SfmlVc got event\n";
			controller->handleSfmlEvent(event);
			if (event.type == sf::Event::Closed) {
				std::cout << "SfmlVc got close Event\n";
				if (window.isOpen()) window.close();
				break;
			}
		}
	}
}

std::vector<std::thread*> SfmlVc::start() {
	if (running_thread != nullptr) return {running_thread};
	std::thread* t = new std::thread(&SfmlVc::loop, this);
	running_thread = t;
	return {t};
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


