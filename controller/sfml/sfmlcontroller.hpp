
/* [bake me]

dependencies["headers"] = [
	"viewcontroller/sfml>>headers",
	"controller>>headers",
	"model/entity>>headers",
	"event>>headers",
	"util/ccq>>headers",
	"util/sleepable>>headers",
]

[stop baking] */

#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>

#include "SFML/Graphics.hpp"
#include "yorel/multi_methods.hpp"

#include "controller/controller.hpp"
#include "viewcontroller/sfml/sfmlvc.hpp"
#include "model/entity/entity.hpp"
#include "event/event.hpp"
#include "util/ccq/ccq.hpp"
#include "util/flag/flag.hpp"
#include "util/sleepable/sleepable.hpp"
#include "model/model_state.hpp"

#pragma once

namespace si {
class Game;

namespace controller {

class SfmlController;

// Multimethods stuff
using yorel::multi_methods::virtual_;
MULTI_METHOD(_handle_event, void, si::controller::SfmlController*, const virtual_<si::Event>&);


class SfmlController: public Controller {
public:
	
	SfmlController(Game* g, bool _concurrent=false);
	
	std::vector<std::thread*> start();
	
	void handle_event(Event* e);
	
	Event* get_event();
	
	bool is_concurrent() { return concurrent; }
	
	void wait_until_done() { done.wait(); }
	
	// make sure all the handlers can actually access this class
	template <typename T>
	friend class _handle_event_specialization;
	
	friend class vc::SfmlVc;
	
private:
	void loop();
	
	util::CCQueue<Event*> input_queue; // input from model (or SfmlVc)
	util::CCQueue<Event*> output_queue; // output to model
	util::Sleepable slp;
	
	model::State state = model::State::PRE_WAIT;
	
	Game* game;
	int my_player = -1;
	
	vc::SfmlVc* handle;
	bool concurrent;
	util::Flag done;
};

}
}


