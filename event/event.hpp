
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
	
	util::Vector2D_d dir;
	si::model::Entity* entity;
};

class Tick: public Event {
	MM_CLASS(Tick, Event);
};

class Redraw: public Event {
	MM_CLASS(Redraw, Event);
};

// TODO:
//  - scene change?
//  - ...
//  - comment about implicit events in dispatchers within model

}
