
/* [bake me]

dependencies["headers"] = [
	"event>>headers"
]

[stop baking] */

#pragma once

#include <thread>
#include <vector>

#include "event/event.hpp"

namespace si {
namespace controller {

class Controller {
public:
	virtual std::vector<std::thread*> start() = 0;
	
	virtual void handle_event(si::Event* e) = 0;
	
	// return nullptr when there is none
	virtual Event* get_event() = 0;
	
	virtual bool is_concurrent() = 0;
	
	virtual void wait_until_done() = 0;
	
	virtual ~Controller() {};
};

}
}
