
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
namespace view {

class View {
public:
	virtual std::vector<std::thread*> start() = 0;
	
	virtual void wake_up() {}
	
	// Events are pulled from the game
	
	~View() {};
};


}
}
