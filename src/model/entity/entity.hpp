
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


class Earth: public Entity {
public:
	MM_CLASS(Earth, Entity);
	
	Earth() { MM_INIT(); }
	
	Earth(double _x, double _y): Entity(_x, _y) {
		MM_INIT();
	}
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

// ATTENTION: Gaps should include the width and height

class Player: public Actor {
public:
	MM_CLASS(Player, Actor);
	
	constexpr static int width = 52;
	constexpr static int height = 32;
	constexpr static int height_from_earth = 50;
	
	Player(double _x, double _y): Actor(_x, _y) {
		MM_INIT();
		mov.speed = 0.2;
		size.x = width;
		size.y = height;
	}
	
	Bullet* b = nullptr;
	int score = 0;
	int lives = 3;
};


class Alien: public Actor {
public:
	MM_CLASS(Alien, Actor);
	
	constexpr static int width = 52;
	constexpr static int height = 32;
	constexpr static double gap_x = 75;
	constexpr static double gap_y = 60;
	
	Alien(double _x, double _y, unsigned int _col, unsigned int _row): 
			Actor(_x, _y), col(_col), row(_row) {
		MM_INIT();
		
		size.x = width;
		size.y = height;
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
		mov = Movement(1.2, util::Vector2D_d(0, -1));
	}
	
	Player* p;
};

class Bomb: public Projectile {
public:
	MM_CLASS(Bomb, Projectile);
	
	Bomb(Alien* a) {
		MM_INIT();
		size.x = 12;
		size.y = 28;
		pos.x = a->pos.x + ((a->size.x - size.x)/2);
		pos.y = a->pos.y + a->size.y;
		mov = Movement(0.3, util::Vector2D_d(0, 1));
	}
};

}
}

#pragma GCC diagnostic pop
