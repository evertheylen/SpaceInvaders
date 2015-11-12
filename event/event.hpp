
/* [bake me]

dependencies["headers"] = [
]

[stop baking] */

#pragma once

namespace si {

enum EventType {
	MOVE,
	COLLISION
	// ...
};

class Event {
public:
	virtual EventType type() = 0;
};

class Move: public Event {
public:
	EventType type() {
		return EventType::MOVE;
	}
};

}
