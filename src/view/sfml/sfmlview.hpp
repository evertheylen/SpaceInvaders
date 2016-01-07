
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
#include "util/flag/flag.hpp"
#include "model/model_state.hpp"

#include "sfmlresources.hpp"

#pragma once

namespace si {
class Game;

namespace view {

class SfmlView;

// Multimethods stuff
using yorel::multi_methods::virtual_;
MULTI_METHOD(_handle_event, void, si::view::SfmlView*, const virtual_<si::Event>&);

MULTI_METHOD(_draw, void, si::view::SfmlView*, const virtual_<si::model::Entity>&);


class SfmlView: public View {
public:
	SfmlView(Game* g, bool _concurrent);
	
	std::vector<std::thread*> start();
	
	// called by game or controller
	// may dispatch the events to the functions in mm_sfmlview.cpp
	void handle_event(Event* e);
	
	bool is_concurrent() { return concurrent; };
	
	void wait_until_done() { done.wait(); };
	
	// make sure all the handlers can actually access this class
	template <typename T>
	friend class _handle_event_specialization;
	
	template <typename T>
	friend class _draw_specialization;
	
	friend class vc::SfmlVc;
	
private:
	void redraw();
	
	void simple_draw(sf::Texture& tex, const model::Entity& e);
	
	util::CCQueue<Event*> queue;
	
	model::State state = model::WAIT;
	void loop();
	
	void text_mode(); // Recap, GameOver, Wait
	void game_mode(); // Playing
	
	Game* game;
	
	// keeps track of all the objects on screen, and their respective entities
	//std::map<si::model::Entity*, sf::Sprite> sprites;
	sf::Sprite backgroundsprite;
	
	// OpenGL can't handle reusing the same textures in multiple threads
	// Constructing this should automatically load the textures
	SfmlResources res;
	
	vc::SfmlVc* handle;
	bool concurrent;
	util::Flag done;
	
	// statistics
	long int ticks = 0;
};

}
}


