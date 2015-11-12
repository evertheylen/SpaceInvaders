
/* [bake me]

dependencies["headers"] = [
	"event>>headers"
]

[stop baking] */

#pragma once

#include "event/event.hpp"

namespace si {
namespace view {

class View {
public:
	virtual void start() = 0;
	
	virtual void handleEvent(si::Event* e) = 0;
	
	~View() {};
};


}
}
