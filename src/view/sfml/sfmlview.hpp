
#pragma once

#include <map>
#include <memory>
#include <condition_variable>
#include <vector>
#include <string>

#include "SFML/Graphics.hpp"
#include "yorel/multi_methods.hpp"

#include "view/view.hpp"
#include "event/event.hpp"
#include "viewcontroller/sfml/sfmlvc.hpp"
#include "model/entity/entity.hpp"
#include "util/ccq/ccq.hpp"
#include "util/flag/flag.hpp"
#include "util/rwlock/rwlock.hpp"
#include "model/model_state.hpp"

#include "sfmlresources.hpp"


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
	
	bool is_concurrent() const { return concurrent; };
	
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
	
	model::Phase phase = model::PRE_WAIT;
	void loop();
	
	// in contrast to the model, these aren't loops
	// they are simple functions that just run something once
	void wait();
	void playing_cc();
	void playing_syn();
	void recap();
	void gameover();
	
	void resize(int width, int height, bool blur=false);
	void draw_text(int x, int y, const std::string& s);
	void draw_overlay();
	void draw_score();
	
	void phase_change(model::Phase newphase);
	
	void handle_all_events();
	
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
	util::RWLock::TimepointT timestamp;
	
	// statistics
	long int ticks = 0;
};

}
}


