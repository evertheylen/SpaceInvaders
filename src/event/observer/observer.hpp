
#pragma once

#include "model/model_state.hpp"


namespace si {

class Event;

// aka ModelObserver
class Observer {
public:
	virtual void handle_event(Event* e) = 0;
	
	bool update_state(const si::model::State& newstate);
	
protected:
	model::State model_state;
};

}
