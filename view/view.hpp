
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
	View(bool _concurrent): concurrent(_concurrent) {}
	
	virtual std::vector<std::thread*> start() = 0;
	
	virtual void handleEvent(Event* e) = 0;
	
	bool isConcurrent() { return concurrent; }
	
	~View() {};
	
private:
	bool concurrent;
};


}
}
