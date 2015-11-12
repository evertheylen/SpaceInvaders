
/* [bake me]

dependencies["headers"] = [
	"viewcontroller/sfml>>headers",
	"controller>>headers"
]

[stop baking] */

#include "SFML/Graphics.hpp"

#include "controller/controller.hpp"
#include "viewcontroller/sfml/sfmlvc.hpp"

#pragma once

namespace si {
class Game;

namespace controller {


class SfmlController: public si::vc::SfmlBase, public Controller {
public:
	SfmlController(Game* g);
	
	void start();
	
	void handleSfmlEvent(sf::Event& e);
private:
	Game* game;
};

}
}


