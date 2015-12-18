
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
#include "util/sleepable/sleepable.hpp"
#include "model/model_state.hpp"

#pragma once

namespace si {
class Game;

namespace controller {

class SfmlController;

// Multimethods stuff
using yorel::multi_methods::virtual_;
MULTI_METHOD(_handleEvent, void, si::controller::SfmlController*, virtual_<si::Event>&);


class SfmlController: public si::vc::SfmlBase, public Controller {
public:
	
	SfmlController(Game* g);
	
	std::vector<std::thread*> start();
	
	void loop();
	
	void handleEvent(Event* e);
	
	// make sure all the handlers can actually access this class
	template <typename T>
	friend class _handleEvent_specialization;
	
private:
	util::CCQueue<Event*> queue;
	util::Sleepable slp;
	
	model::State state = model::State::PRE_WAIT;
	
	Game* game;
	int my_player = -1;
	
};

}
}


