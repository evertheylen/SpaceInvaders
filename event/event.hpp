
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

class Tick: public Event {
	MM_CLASS(Tick, Event);
	
	Event* clone() {
		return new Tick(*this);
	}
};

class Redraw: public Event {
	MM_CLASS(Redraw, Event);
	
	Event* clone() {
		return new Redraw(*this);
	}
};

// TODO:
//  - scene change?
//  - ...
//  - comment about implicit events in dispatchers within model

}
