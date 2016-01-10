
#pragma once

#include <thread>
#include <vector>

#include "event/event.hpp"
#include "event/observer/observer.hpp"

namespace si {
namespace view {

class View: public Observer {
public:
	virtual std::vector<std::thread*> start() = 0;
	
	virtual bool is_concurrent() const = 0;
	
	virtual void wait_until_done() = 0;
	
	~View() {};
};


}
}
