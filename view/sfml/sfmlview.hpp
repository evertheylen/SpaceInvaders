
/* [bake me]

dependencies["headers"] = [
	"viewcontroller/sfml>>headers",
	"view>>headers",
	"event>>headers",
]

dependencies["build_objects"] = [
	"game>>headers"
]

[stop baking] */

#include "view/view.hpp"
#include "event/event.hpp"
#include "viewcontroller/sfml/sfmlvc.hpp"

#pragma once

namespace si {
class Game;

namespace view {


class SfmlView: public si::vc::SfmlBase, public View {
public:
	SfmlView(Game* g);
	
	void start();
	
	void handleEvent(si::Event* e);
private:
	Game* game;
};

}
}


