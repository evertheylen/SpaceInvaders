
/* [bake me]

dependencies["headers"] = [
	"util>>headers",
	"util/stopwatch>>headers",
]

[stop baking] */

#pragma once

#include "yorel/multi_methods.hpp"

#include "util/util.hpp"
#include "movement.hpp"


namespace si {
namespace model {


class Entity: public yorel::multi_methods::selector {
public:
	MM_CLASS(Entity);
	
	Entity(double _x, double _y): pos(_x, _y) {
		MM_INIT();
	}
	
	Entity(const Entity& e) = delete;  // so the compiler warns me about copies (don't contain MM_INIT yet)
	
	virtual ~Entity() {}
	
	util::Vector2D_d pos;
	Movement mov;  // some other physics term?
	               // default goes nowhere
};


class Player: public Entity {
public:
	MM_CLASS(Player, Entity);
	
	Player(double _x, double _y): Entity(_x, _y) {
		MM_INIT();
	}
};


class Alien: public Entity {
public:
	MM_CLASS(Alien, Entity);
	
	Alien(double _x, double _y): Entity(_x, _y) {
		MM_INIT();
	}
};


class Bullet: public Entity {
public:
	MM_CLASS(Bullet, Entity);
	
	Bullet(double _x, double _y): Entity(_x, _y) {
		MM_INIT();
		mov = Movement(0.4, util::Vector2D_d(0, 1)); // TODO magic constant
	}
};

class Bomb: public Entity {
public:
	MM_CLASS(Bomb, Entity);
	
	Bomb(double _x, double _y): Entity(_x, _y) {
		MM_INIT();
		mov = Movement(0.1, util::Vector2D_d(0, -1)); // TODO magic constant
	}
};

}
}
