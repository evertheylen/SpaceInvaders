
#pragma once

#include <thread>
#include <vector>

#include "event/event.hpp"

namespace si {
namespace view {

class View {
public:
	virtual std::vector<std::thread*> start() = 0;
	
	virtual void handle_event(Event* e) = 0;
	
	virtual bool is_concurrent() = 0;
	
	virtual void wait_until_done() = 0;
	
	~View() {};
};


}
}
