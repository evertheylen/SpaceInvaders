
#pragma once

#include <thread>
#include <vector>

#include "event/event.hpp"
#include "event/observer/observer.hpp"

namespace si {
namespace controller {

class Controller: public Observer {
public:
	virtual std::vector<std::thread*> start() = 0;
	
	// return nullptr when there is none
	virtual Event* get_event() = 0;
	
	virtual bool is_concurrent() const = 0;
	
	virtual void wait_until_done() = 0;
	
	virtual ~Controller() {};
};

}
}
