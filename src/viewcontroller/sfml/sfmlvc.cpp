
#include "sfmlvc.hpp"

#include <iostream>

#include "controller/sfml/sfmlcontroller.hpp"
#include "view/sfml/sfmlview.hpp"

using namespace si;
using namespace si::vc;


void SfmlVc::init(int width, int height) {
	window = new sf::RenderWindow(sf::VideoMode(width, height), "Space Invaders");
	window->setActive(false);
}

SfmlVc::~SfmlVc() {
	delete window;
}

void SfmlVc::loop(util::Flag& f) {
	init();
	std::cout << "SfmlVc Input loop started\n";
	window->setActive(false); // drawing happens from view
	f.set();
	if (controller != nullptr) {
		sf::Event e;
		while (window->isOpen() and window->waitEvent(e)) {
			if (e.type != sf::Event::MouseMoved) {
				if (e.type == sf::Event::Closed) {
					std::cout << "SfmlVc got close Event\n";
					controller->handle_event(new si::SfmlExit);
					return;
				} else {
					controller->handle_event(new si::SfmlInput(e));
				}
			}
		}
	} else {
		// you could call this a mini-controller because sfml needs one
		view->state = si::model::WAIT;
		sf::Event e;
		while (window->isOpen() and window->waitEvent(e)) {
			if (e.type == sf::Event::Closed) {
				std::cout << "SfmlVc got close Event\n";
				view->handle_event(new si::SfmlExit);
				//if (window.isOpen()) window.close();
				return;
			}
		}
	}
}

std::vector<std::thread*> SfmlVc::start() {
	util::Flag f;
	if (running_thread != nullptr) return {running_thread};
	std::thread* t = new std::thread(&SfmlVc::loop, this, std::ref(f));
	running_thread = t;
	f.wait();
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


