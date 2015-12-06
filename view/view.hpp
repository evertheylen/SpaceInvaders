
/* [bake me]

dependencies["headers"] = [
	"event>>headers"
]

[stop baking] */

#pragma once

#include <thread>

#include "event/event.hpp"

namespace si {
namespace view {

class View {
public:
	virtual std::thread* start() = 0;
	
	virtual void handleEvent(si::Event* e) = 0;
	
	~View() {};
};


}
}
