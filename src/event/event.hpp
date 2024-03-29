
#pragma once

// only used in SFML specific events:
#include "SFML/Window.hpp"

#include "yorel/multi_methods.hpp"

#include "util/util.hpp"
#include "util/flag/flag.hpp"
#include "model/model_state.hpp"
#include "model/entity/entity.hpp"
#include "event/observer/observer.hpp"

// YOMM11 gives these warnings
#pragma GCC diagnostic ignored "-Wunused-value"

namespace si {

//BEGIN basic events and utils

// WARNING Always call MM_INIT() in every constructor!
// WARNING Do not rely on synthesized constructors!

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
	
	virtual Event* clone() const = 0;
	
	virtual const char* name() const = 0;
};


namespace model {
	// forward declaration
	class Entity;
	class Level;
}


#define SIMPLE_NAME(argname) \
const char* name() const { \
  return argname ; \
}


#define SIMPLE_EVENT(name)            \
class name: public Event {            \
public:                               \
	MM_CLASS(name, Event);            \
	SIMPLE_CTORS(name);               \
	SIMPLE_NAME( #name );              \
	Event* clone() const {            \
		return new name(*this);       \
	}                                 \
};

//END


//BEGIN Main events

// Events below are given in the order they will most likely be spawned

SIMPLE_EVENT(Init);

// ModelStateChange(WAIT)

SIMPLE_EVENT(Ready);

// ModelStateChange(PLAYING)


// Has a more special status in some contexts
// Basically, all events are in someway generated by user input. However, a Tick event can be
// spawned independently from user input. This is important because Ticks may flood the
// communication channels in multithreaded environments.
// If there were more events like these, I would subclass them from Tick (and possibly
// rename Tick). However, so far that has not been necessary.
SIMPLE_EVENT(Tick);

// ModelStateChange(RECAP)

SIMPLE_EVENT(VCStop);


class ModelStateChange: public Event {
public: 
	MM_CLASS(ModelStateChange, Event);
	SIMPLE_NAME("ModelStateChange")
	
	ModelStateChange(si::model::State _state):
			state(_state) {
		MM_INIT();
	}
	
	ModelStateChange(const ModelStateChange& n):
			Event(n), state(n.state) {
		MM_INIT();
	}
	
	Event* clone() const {
		return new ModelStateChange(*this);
	}
	
	si::model::State state;
};

//END


//BEGIN Events concerning a player (Controller --> Model)

class PlayerEvent: public Event {
public:
	MM_CLASS(PlayerEvent, Event);
	SIMPLE_NAME("PlayerEvent")
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
	SIMPLE_NAME( #name ) \
	name(unsigned int _ID):\
			PlayerEvent(_ID) {\
		MM_INIT();\
	}\
	name(const name& n):\
			PlayerEvent(n) {\
		MM_INIT();\
	}\
	Event* clone() const {\
		return new name(*this);\
	}\
};

// also see AskPlayer

PLAYER_EVENT(CreatePlayer);

PLAYER_EVENT(ReleasePlayer);

// shit gets real
PLAYER_EVENT(Fire);

class SetDirection: public PlayerEvent {
public: 
	MM_CLASS(SetDirection, PlayerEvent);
	SIMPLE_NAME("SetDirection")
	SetDirection(unsigned int _ID, si::util::Vector2D_d _dir):
			PlayerEvent(_ID), dir(_dir) {
		MM_INIT();
	}
	
	SetDirection(const SetDirection& n):
			PlayerEvent(n), dir(n.dir) {
		MM_INIT();
	}
	
	Event* clone() const {
		return new SetDirection(*this);
	}
	
	si::util::Vector2D_d dir;
};

//END


//BEGIN Events concerning other entities (Model --> View)

#define ENTITY_EVENT(name, entity_type)       \
class name: public Event { \
public: \
	MM_CLASS(name, Event);\
	SIMPLE_NAME( #name ) \
	name(entity_type * _entity):\
			entity(_entity) {\
		MM_INIT();\
	}\
	name(const name& n):\
			Event(n) {\
		MM_INIT();\
	}\
	Event* clone() const {\
		return new name(*this);\
	}\
	entity_type * entity;\
};

// TODO use these or remove them

ENTITY_EVENT(Death, model::Entity);

ENTITY_EVENT(PlayerShoots, model::Player);

ENTITY_EVENT(AlienShoots, model::Alien);

class Collision: public Event {
public:
	MM_CLASS(Collision, Event);
	SIMPLE_NAME("Collision");
	
	Collision(model::Entity* _a, model::Entity* _b):
			a(_a), b(_b) {
		MM_INIT();
	}
	
	Collision(const Collision& p):
			Event(p), a(p.a), b(p.b) {
		MM_INIT();
	}
	
	Event* clone() const {
		return new Collision(*this);
	}
	
	model::Entity* a;
	model::Entity* b;
};

//END



//BEGIN Two way communication
// usually a view or controller passes an event like this one to the model,
// which replies through the specified observer pointer

class Callback: public Event {
public:
	MM_CLASS(Callback, Event);
	SIMPLE_NAME("Callback");
	
	Callback(Observer* _obv): obv(_obv) {
		MM_INIT();
	}
	
	Callback(const Callback& c):
			Event(c), obv(c.obv) {
		MM_INIT();
	}
	
	Event* clone() const {
		return new Callback(*this);
	}
	
	Observer* obv;
};

#define CALLBACK_EVENT(name)       \
class name: public Callback { \
public: \
	MM_CLASS(name, Callback);\
	SIMPLE_NAME( #name ) \
	name(Observer* _obv):\
			Callback(_obv) {\
		MM_INIT();\
	}\
	name(const name& n):\
			Callback(n) {\
		MM_INIT();\
	}\
	Event* clone() const {\
		return new name(*this);\
	}\
};

CALLBACK_EVENT(AskPlayer);

//END



//BEGIN SFML specific events (TODO put in other file)

SIMPLE_EVENT(SfmlEvent);

enum DisplayState { OK, ERROR };

class DisplayText: public SfmlEvent {
public:
	MM_CLASS(DisplayText, SfmlEvent);
	SIMPLE_NAME("DisplayText")
	
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
	SIMPLE_NAME("SfmlInput")
	
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
	SIMPLE_NAME("SfmlExit")
	
	Event* clone() const {
		return new SfmlExit(*this);
	}
};

class SfmlReady: public SfmlEvent {
public:
	MM_CLASS(SfmlReady, SfmlEvent);
	SIMPLE_CTORS(SfmlReady);
	SIMPLE_NAME("SfmlReady")
	
	Event* clone() const {
		return new SfmlReady(*this);
	}
};

//END


// don't like macro's?
#undef SIMPLE_EVENT
#undef PLAYER_EVENT
#undef SIMPLE_CTORS
#undef CALLBACK_EVENT

// Above are all the definitions of all the possible subclasses of Event, technically.
// However, you could argue (and this is also how I originally imagined it) that there
// are also implicit events within the open multimethods in model. These implicit Events
// are never instantiated, but are immediatly handled in some onFoo(Bar) function.
// An example is: TODO

}

#pragma GCC diagnostic pop
