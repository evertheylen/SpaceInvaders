
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
	
	virtual void handleEvent(si::Event* e) = 0;
	
	virtual ~Controller() {};
};

}
}
