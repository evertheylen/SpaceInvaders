
#include "sfmlvc.hpp"

#include <iostream>

#include "controller/sfml/sfmlcontroller.hpp"
#include "view/sfml/sfmlview.hpp"

using namespace si::vc;


void SfmlBase::start() {
	assert(handle != nullptr);
	handle->start();
}




void SfmlVc::start() {
	if (running) return;
	running = true;
	assert(controller != nullptr);
	// no need to assert that there is a view
	
	std::cout << "SfmlVc Started\n";
	
	sf::Event event;
	while(window.waitEvent(event)) {
		std::cout << "SfmlVc got event\n";
		// SFML likes blocking so we do too
		controller->handleSfmlEvent(event);
		if (event.type == sf::Event::Closed) {
			window.close();
			break;
		}
	}
	running = false;
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


