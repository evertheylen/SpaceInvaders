
#include "sfmlcontroller.hpp"

#include "game/game.hpp"

using namespace si::controller;
using namespace si;

SfmlController::SfmlController(Game* g):
		game(g) {}

void SfmlController::start() {
	SfmlBase::start();
}

void SfmlController::handleSfmlEvent(sf::Event& e) {
	
}
