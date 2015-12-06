
/* [bake me]

dependencies["headers"] = [
	"viewcontroller/sfml>>headers",
	"controller>>headers",
	"model/entity>>headers",
]

[stop baking] */

#include <thread>

#include "SFML/Graphics.hpp"

#include "controller/controller.hpp"
#include "viewcontroller/sfml/sfmlvc.hpp"
#include "model/entity/entity.hpp"

#pragma once

namespace si {
class Game;

namespace controller {


class SfmlController: public si::vc::SfmlBase, public Controller {
public:
	SfmlController(Game* g);
	
	std::thread* start();
	
	void handleSfmlEvent(sf::Event& e);
private:
	Game* game;
	si::model::Player* my_player;
};

}
}


