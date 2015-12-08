
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
		
	Entity() {
		MM_INIT();
	}
	
	Entity(double _x, double _y): pos(_x, _y) {
		MM_INIT();
	}
	
	virtual ~Entity() {}
	
	util::Vector2D_d pos;
	Movement mov;  // some other physics term?
	               // default goes nowhere
};


class Player: public Entity {
	MM_CLASS(Player, Entity);
	using Entity::Entity;
};


class Alien: public Entity {
	MM_CLASS(Alien, Entity);
	using Entity::Entity;
};


}
}
