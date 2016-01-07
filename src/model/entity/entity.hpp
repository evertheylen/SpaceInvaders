
#pragma once

#include "yorel/multi_methods.hpp"

#include "util/util.hpp"
#include "movement.hpp"

// YOMM11 gives these warnings
#pragma GCC diagnostic ignored "-Wunused-value"


namespace si {
namespace model {


class Entity: public yorel::multi_methods::selector {
public:
	MM_CLASS(Entity);
	
	Entity() { MM_INIT(); }
	
	Entity(double _x, double _y): pos(_x, _y) {
		MM_INIT();
	}
	
	Entity(const Entity& e) = delete;  // so the compiler warns me about copies (don't contain MM_INIT yet)
	
	virtual ~Entity() {}
	
	util::Vector2D_d pos; // upper left
	util::Vector2D_d size; // pos+size = down right
	Movement mov;  // some other physics term?
	               // default goes nowhere
	bool killme = false;
};

class Actor: public Entity {
public:
	MM_CLASS(Actor, Entity);
	
	Actor() { MM_INIT(); }
	
	Actor(double _x, double _y): Entity(_x, _y) {
		MM_INIT();
	}
};


class Projectile: public Entity {
public:
	MM_CLASS(Projectile, Entity);
	
	Projectile() { MM_INIT(); }
	
	Projectile(double _x, double _y): Entity(_x, _y) {
		MM_INIT();
	}
};


class Bullet;

class Player: public Actor {
public:
	MM_CLASS(Player, Actor);
	
	Player(double _x, double _y): Actor(_x, _y) {
		MM_INIT();
		mov.speed = 0.2;
		size.x = 52;
		size.y = 32;
	}
	
	Bullet* b = nullptr;
};


class Alien: public Actor {
public:
	MM_CLASS(Alien, Actor);
	
	Alien(double _x, double _y, unsigned int _col, unsigned int _row): 
			Actor(_x, _y), col(_col), row(_row) {
		MM_INIT();
		
		size.x = 48;
		size.y = 32;
	}
	
	unsigned int col;
	unsigned int row;
};


class Bullet: public Projectile {
public:
	MM_CLASS(Bullet, Projectile);
	
	Bullet(Player* _p): p(_p) {
		MM_INIT();
		size.x = 2;
		size.y = 14;
		pos.x = p->pos.x + ((p->size.x - size.x)/2);
		pos.y = p->pos.y - size.y;
		mov = Movement(0.4, util::Vector2D_d(0, -1));
	}
	
	Player* p;
};

class Bomb: public Projectile {
public:
	MM_CLASS(Bomb, Projectile);
	
	Bomb(double _x, double _y): Projectile(_x, _y) {
		MM_INIT();
		mov = Movement(0.1, util::Vector2D_d(0, 1)); // TODO magic constant
	}
};

}
}

#pragma GCC diagnostic pop
