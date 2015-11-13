
/* [bake me]

dependencies["headers"] = [
	"util>>headers"
]

[stop baking] */

#pragma once

#include "util/util.hpp"

namespace si {

enum EventType {
	START_MOVE,
	END_MOVE,
	TICK,
	REDRAW,
	// ...
};

class Event {
public:
	virtual EventType type() = 0;
};


// more elegant way to define a type() function
template <EventType T>
class EventBase: public Event {
public:
	EventType type() {
		return T;
	}
};

namespace model {
class Player;
}

class BaseMove {
public:
	BaseMove(util::Direction _dir, si::model::Player* _player): dir(_dir), player(_player) {}
	util::Direction dir;
	si::model::Player* player;
};

class StartMove: public EventBase<EventType::START_MOVE>, public BaseMove {
public:
	using BaseMove::BaseMove;
};


class EndMove: public EventBase<EventType::END_MOVE>, public BaseMove {
public:
	using BaseMove::BaseMove;
};


class Tick: public EventBase<EventType::TICK> {};

class Redraw: public EventBase<EventType::REDRAW> {};

}
