
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

#include "sfmlresources.hpp"

#pragma once

namespace si {
class Game;

namespace view {


class SfmlView: public si::vc::SfmlBase, public View {
public:
	SfmlView(Game* g);
	
	std::vector<std::thread*> start();
	
	void wake_up();
	
	static void load_resources();
	static void unload_resources();
	
private:
	void handleEvent(Event* e);
	
	void redraw();
	
	void sleep();
	
	void loop();

	
	Game* game;
	
	// by default nullptr
	static SfmlResources* res;
	
	// to go to sleep / wake up
	// avoid spurious wake ups
	std::atomic<bool> sleep_notified;
	std::mutex sleep_lock;
	std::condition_variable sleep_cv;
	
	// keeps track of all the objects on screen, and their respective entities
	std::map<si::model::Entity*, sf::Sprite> sprites;
	sf::Sprite backgroundsprite;
};

}
}


