
/* [bake me]

dependencies["headers"] = [
	"util>>headers"
]

[stop baking] */

#pragma once

// only used in SFML specific events:
#include "SFML/Window.hpp"

#include "yorel/multi_methods.hpp"

#include "util/util.hpp"
#include "model/model_state.hpp"
#include "model/entity/entity.hpp"

namespace si {

// WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
// WARNING Always call MM_INIT() in every constructor!     WARNING
// WARNING DO NOT RELY ON SYNTHESIZED CONSTRUCTORS!        WARNING
// WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING

// Disadvantage of yomm11: verbose constructors
// I am more than willing to pay that price though

#define SIMPLE_CTORS(classname)     \
classname() {                       \
  MM_INIT();                        \
}                                   \
classname(const classname& c) {     \
  MM_INIT();                        \
}


// Event cooperates with yomm11
class Event: public yorel::multi_methods::selector {
public:
	MM_CLASS(Event);
	
	SIMPLE_CTORS(Event);
	
	virtual Event* clone() = 0;
};


namespace model {
	// forward declaration
	class Entity;
}


#define SIMPLE_EVENT(name)      \
class name: public Event {             \
public:                               \
	MM_CLASS(name, Event);             \
	SIMPLE_CTORS(name);               \
	Event* clone() {                  \
		return new name(*this);       \
	}                                 \
};



// Events below are given in the order they will most likely be spawned

SIMPLE_EVENT(Init);

SIMPLE_EVENT(ModelStart);

SIMPLE_EVENT(WaitPlayers);

class PlayerEvent: public Event {
public:
	MM_CLASS(PlayerEvent, Event);
	
	PlayerEvent(unsigned int _ID):
			ID(_ID) {
		MM_INIT();
	}
	
	PlayerEvent(const PlayerEvent& p):
			ID(p.ID) {
		MM_INIT();
	}
	
	unsigned int ID;
};

#define PLAYER_EVENT(name)       \
class name: public PlayerEvent { \
public: \
	MM_CLASS(name, PlayerEvent);\
	name(unsigned int _ID):\
			PlayerEvent(_ID) {\
		MM_INIT();\
	}\
	name(const name& n):\
			PlayerEvent(n) {\
		MM_INIT();\
	}\
	Event* clone() {\
		return new name(*this);\
	}\
};

PLAYER_EVENT(CreatePlayer);

PLAYER_EVENT(ReleasePlayer);

// shit gets real
PLAYER_EVENT(Fire);

class SetDirection: public PlayerEvent {
public: 
	MM_CLASS(SetDirection, PlayerEvent);
	SetDirection(unsigned int _ID, si::util::Vector2D_d _dir):
			PlayerEvent(_ID), dir(_dir) {
		MM_INIT();
	}
	
	SetDirection(const SetDirection& n):
			PlayerEvent(n), dir(n.dir) {
		MM_INIT();
	}
	
	Event* clone() {
		return new SetDirection(*this);
	}
	
	si::util::Vector2D_d dir;
};

// class EntityEvent: public Event {
// public:
// 	MM_CLASS(EntityEvent, Event);
// 	
// 	EntityEvent(model::Entity* _entity):
// 			entity(_entity) {
// 		MM_INIT();
// 	}
// 	
// 	EntityEvent(const EntityEvent& p):
// 			entity(p.entity) {
// 		MM_INIT();
// 	}
// 	
// 	model::Entity* entity;
// };


#define ENTITY_EVENT(name, entity_type)       \
class name: public Event { \
public: \
	MM_CLASS(name, Event);\
	name(entity_type * _entity):\
			entity(_entity) {\
		MM_INIT();\
	}\
	name(const name& n):\
			Event(n) {\
		MM_INIT();\
	}\
	Event* clone() {\
		return new name(*this);\
	}\
	entity_type * entity;\
};


ENTITY_EVENT(Death, model::Entity);

ENTITY_EVENT(PlayerShoots, model::Player);

ENTITY_EVENT(AlienShoots, model::Alien);

class Collision: public Event {
public:
	MM_CLASS(Collision, Event);
	
	Collision(model::Entity* _a, model::Entity* _b):
			a(_a), b(_b) {
		MM_INIT();
	}
	
	Collision(const Collision& p):
			Event(p), a(p.a), b(p.b) {
		MM_INIT();
	}
	
	model::Entity* a;
	model::Entity* b;
};


class ModelStateChange: public Event {
public: 
	MM_CLASS(ModelStateChange, Event);
	ModelStateChange(si::model::State _state):
			state(_state) {
		MM_INIT();
	}
	
	ModelStateChange(const ModelStateChange& n):
			Event(n), state(n.state) {
		MM_INIT();
	}
	
	Event* clone() {
		return new ModelStateChange(*this);
	}
	
	si::model::State state;
};


SIMPLE_EVENT(Ready);

SIMPLE_EVENT(LevelStart);

// Has a more special status in some contexts
// Basically, all events are in someway generated by user input. However, a Tick event can be
// spawned independently from user input. This is important because Ticks may flood the
// communication channels in multithreaded environments.
// If there were more events like these, I would subclass them from Tick (and possibly
// rename Tick). However, so far that has not been necessary.
SIMPLE_EVENT(Tick);

SIMPLE_EVENT(LevelEnd);

SIMPLE_EVENT(Recap);

class GameStop: public Event {
public:
	MM_CLASS(GameStop, Event);
	
	GameStop(bool _victory): victory(_victory) {
		MM_INIT();
	}
	
	GameStop(const GameStop& g): Event(g), victory(g.victory) {
		MM_INIT();
	}
	
	Event* clone() {
		return new GameStop(*this);
	}
	
	bool victory;
};

SIMPLE_EVENT(VCStop);

SIMPLE_EVENT(Quit);




// Below are sfml specific events (TODO put in other file)

SIMPLE_EVENT(SfmlEvent);

enum DisplayState { OK, ERROR };

class DisplayText: public SfmlEvent {
public:
	MM_CLASS(DisplayText, SfmlEvent);
	DisplayText(const std::string& msg, DisplayState _state = DisplayState::OK):
			message(msg), state(_state) {
		MM_INIT();
	}
	
	DisplayText(const DisplayText& d):
			SfmlEvent(d), message(d.message), state(d.state) {
		MM_INIT();
	}
	
	std::string message;
	DisplayState state;
};

class SfmlInput: public SfmlEvent {
public:
	MM_CLASS(SfmlInput, SfmlEvent);
	SfmlInput(const sf::Event& e):
			event(e) {
		MM_INIT();
	}
	
	SfmlInput(const SfmlInput& d):
			SfmlEvent(d), event(d.event) {
		MM_INIT();
	}
	
	sf::Event event;
};

class SfmlExit: public SfmlEvent {
public:
	MM_CLASS(SfmlExit, SfmlEvent);
	SIMPLE_CTORS(SfmlExit);
	Event* clone() {
		return new SfmlExit(*this);
	}
};

class SfmlReady: public SfmlEvent {
public:
	MM_CLASS(SfmlReady, SfmlEvent);
	SIMPLE_CTORS(SfmlReady);
	Event* clone() {
		return new SfmlReady(*this);
	}
};




// don't like macro's?
#undef SIMPLE_EVENT
#undef PLAYER_EVENT
#undef SIMPLE_CTORS

// Above are all the definitions of all the possible subclasses of Event, technically.
// However, you could argue (and this is also how I originally imagined it) that there
// are also implicit events within the open multimethods in model. These implicit Events
// are never instantiated, but are immediatly handled in some onFoo(Bar) function.
// An example is: TODO

}
