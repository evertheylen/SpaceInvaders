
#include "sfmlcontroller.hpp"

#include "game/game.hpp"
#include "util/util.hpp"

using namespace si::controller;
using namespace si;

SfmlController::SfmlController(Game* g):
		game(g) {}


std::thread* SfmlController::start() {
	std::cout << "Controller has started\n";
	// Model should not be started yet!
	// otherwise the model_lock will be locked until the end of the game
	game->model_lock();
	my_player = game->get_model().get_player();
	game->model_unlock();
	
	std::thread* t = SfmlBase::start();
	std::cout << "Controller done\n";
	return t;
}

void SfmlController::handleSfmlEvent(sf::Event& e) {
	//std::cout << "controller got event\n";
	switch (e.type) {
		// key pressed
		case sf::Event::KeyPressed:
			switch(e.key.code) {
				case sf::Keyboard::Left:
					// TODO memory leaks
					game->notifyModel(new SetMovement(util::WEST, my_player));
					break;
				case sf::Keyboard::Right:
					game->notifyModel(new SetMovement(util::EAST, my_player));
					break;
				default: break;
			}
			break;
		case sf::Event::KeyReleased:
			game->notifyModel(new SetMovement(util::HOLD, my_player));
		// we don't process other types of events
		default:
			break;
	}
}
