
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
#include "yorel/multi_methods.hpp"

#include "view/view.hpp"
#include "event/event.hpp"
#include "viewcontroller/sfml/sfmlvc.hpp"
#include "model/entity/entity.hpp"
#include "util/ccq/ccq.hpp"
#include "model/model_state.hpp"

#include "sfmlresources.hpp"

#pragma once

namespace si {
class Game;

namespace view {

class SfmlView;

// Multimethods stuff
using yorel::multi_methods::virtual_;
MULTI_METHOD(_handleEvent, void, si::view::SfmlView*, virtual_<si::Event>&);


class SfmlView: public si::vc::SfmlBase, public View {
public:
	SfmlView(Game* g, bool _concurrent);
	
	std::vector<std::thread*> start();
	
	static void load_resources();
	static void unload_resources();
	
	// called by game or controller
	// may dispatch the events to the functions in mm_sfmlview.cpp
	void handleEvent(Event* e);
	
	// make sure all the handlers can actually access this class
	template <typename T>
	friend class _handleEvent_specialization;
	
	friend class vc::SfmlVc;
	
	
private:
	void redraw();
	
	util::CCQueue<Event*> queue;
	
	model::State state = model::State::WAIT;
	void loop();
	
	Game* game;
	
	// keeps track of all the objects on screen, and their respective entities
	std::map<si::model::Entity*, sf::Sprite> sprites;
	sf::Sprite backgroundsprite;
	
	// OpenGL can't handle reusing the same textures in multiple threads
	// Constructing this should automatically load the textures
	SfmlResources res;
	
	// statistics
	long int ticks = 0;
};

}
}


