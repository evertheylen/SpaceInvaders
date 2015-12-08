
/* [bake me]

dependencies["headers"] = [
	"util>>headers"
]

[stop baking] */

#pragma once

#include "yorel/multi_methods.hpp"

#include "util/util.hpp"

namespace si {

// Event cooperates with yomm11
class Event: public yorel::multi_methods::selector {
public:
	MM_CLASS(Event);
	
	Event() {
		MM_INIT();
	}
	
	virtual Event* clone() = 0;
};


namespace model {
	// forward declaration
	class Entity;
}

class SetMovement: public Event {
public:
	MM_CLASS(SetMovement, Event);
	
	SetMovement(util::Vector2D_d _dir, si::model::Entity* e):
		dir(_dir), entity(e) {}
	
	Event* clone() {
		return new SetMovement(*this);
	}
	
	util::Vector2D_d dir;
	si::model::Entity* entity;
};


#define SIMPLE_EVENT(name)            \
class name: public Event {            \
	MM_CLASS(name, Event);            \
	                                  \
	Event* clone() {                  \
		return new name(*this);       \
	}                                 \
};



SIMPLE_EVENT(Init);

SIMPLE_EVENT(GameStart);

SIMPLE_EVENT(Tick); // has a more special status in some contexts

SIMPLE_EVENT(GameStop);

SIMPLE_EVENT(Quit);

// don't like macro's?
#undef SIMPLE_EVENT

// Above are all the definitions of all the possible subclasses of Event, technically.
// However, you could argue (and this is also how I originally imagined it) that there
// are also implicit events within the open multimethods in model. These implicit Events
// are never instantiated, but are immediatly handled in some onFoo(Bar) function.
// An example is: TODO

// TODO:
//  - ...
//  - comment about implicit events in dispatchers within model

}
