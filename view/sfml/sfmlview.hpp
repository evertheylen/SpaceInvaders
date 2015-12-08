
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
#include <condition_variable>
#include <vector>

#include "SFML/Graphics.hpp"

#include "view/view.hpp"
#include "event/event.hpp"
#include "viewcontroller/sfml/sfmlvc.hpp"
#include "model/entity/entity.hpp"
#include "util/ccq/ccq.hpp"

#include "sfmlresources.hpp"

#pragma once

namespace si {
class Game;

namespace view {


class SfmlView: public si::vc::SfmlBase, public View {
public:
	SfmlView(Game* g, bool _concurrent);
	
	std::vector<std::thread*> start();
	
	static void load_resources();
	static void unload_resources();
	
	// called by game
	void handleEvent(Event* e);
	
private:
	void doEvent(Event* e);
	
	void redraw();
	
	util::CCQueue<Event*> queue;
	//void sleep();
	//void wake_up();
	
	void loop();
	
	Game* game;
	
	// by default nullptr
	static SfmlResources* res;
	
	// to go to sleep / wake up
	// spurious wakeups may happen, but the CCQ will be empty
	std::mutex sleep_lock;
	std::condition_variable sleep_cv;
	
	// keeps track of all the objects on screen, and their respective entities
	std::map<si::model::Entity*, sf::Sprite> sprites;
	sf::Sprite backgroundsprite;
};

}
}


