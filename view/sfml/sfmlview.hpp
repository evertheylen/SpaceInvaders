
/* [bake me]

dependencies["headers"] = [
	"viewcontroller/sfml>>headers",
	"view>>headers",
	"event>>headers",
	"model/entity>>headers",
]

dependencies["build_objects"] = [
	"game>>headers"
]

[stop baking] */

#include <map>
#include <memory>

#include "SFML/Graphics.hpp"

#include "view/view.hpp"
#include "event/event.hpp"
#include "viewcontroller/sfml/sfmlvc.hpp"
#include "model/entity/entity.hpp"

#include "sfmlresources.hpp"

#pragma once

namespace si {
class Game;

namespace view {


class SfmlView: public si::vc::SfmlBase, public View {
public:
	SfmlView(Game* g);
	
	std::thread* start();
	
	void handleEvent(si::Event* e);
	
	void redraw();
	
	static void load_resources();
	static void unload_resources();
	
private:
	Game* game;
	
	static SfmlResources* res;
	
	// keeps track of all the objects on screen, and their respective entities
	std::map<si::model::Entity*, sf::Sprite> sprites;
	sf::Sprite backgroundsprite;
};

}
}


