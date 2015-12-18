
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
	// drawing could be done from another thread, so disable the window
	window.setActive(false);
}

void SfmlVc::loop() {
	std::cout << "SfmlVc Input loop started\n";
	if (controller != nullptr) {
		sf::Event e;
		while (window.isOpen() and window.waitEvent(e)) {
			if (e.type == sf::Event::Closed) {
				std::cout << "SfmlVc got close Event\n";
				controller->handleEvent(new si::SfmlExit);
				return;
			} else {
				controller->handleEvent(new si::SfmlInput(e));
			}
		}
	} else {
		view->state = si::model::PLAYING;
		sf::Event e;
		while (window.isOpen() and window.waitEvent(e)) {
			if (e.type == sf::Event::Closed) {
				std::cout << "SfmlVc got close Event\n";
				view->handleEvent(new SfmlExit);
				//if (window.isOpen()) window.close();
				return;
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


