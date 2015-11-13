
/* [bake me]

dependencies["headers"] = [
]

[stop baking] */

#pragma once

namespace si {
namespace model {

enum EntityType {
	PLAYER, ALIEN
};

class Entity {
public:
	virtual EntityType type() = 0;
	
	Entity() = default;
	Entity(double _x, double _y): x(_x), y(_y) {}
	
	double x = 0;
	double y = 0;
};

// same trick as for Events
template <EntityType T>
class EntityBase: public Entity {
public:
	EntityType type() {
		return T;
	}
	using Entity::Entity;
};

class Player: public EntityBase<EntityType::PLAYER> {
};

class Alien: public EntityBase<EntityType::ALIEN> {
};


}
}
