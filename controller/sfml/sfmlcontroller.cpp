
#include "sfmlcontroller.hpp"

#include "game/game.hpp"
#include "util/util.hpp"

using namespace si::controller;
using namespace si;

SfmlController::SfmlController(Game* g):
		game(g) {}

void SfmlController::start() {
	game->model_mutex.lock();
	my_player = game->get_model().get_player();
	game->model_mutex.unlock();
	
	SfmlBase::start();
}

void SfmlController::handleSfmlEvent(sf::Event& e) {
	switch (e.type) {
		// key pressed
		case sf::Event::KeyPressed:
			switch(e.key.code) {
				case sf::Keyboard::Left:
					// TODO memory leaks
					game->notifyModel(new StartMove(util::Direction::WEST, my_player));
					break;
				case sf::Keyboard::Right:
					game->notifyModel(new StartMove(util::Direction::EAST, my_player));
					break;
				default: break;
			}
			break;

		// we don't process other types of events
		default:
			break;
	}
}
