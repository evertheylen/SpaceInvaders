
#include <cassert>

#include "yorel/multi_methods.hpp"

using yorel::multi_methods::virtual_;

#include "model.hpp"
#include "game/game.hpp"

using namespace si;
using namespace si::model;

namespace si {
namespace model {


// === Event handling =============================================================================

void Model::handle_event(Event* e) {
	_handle_event(this, *e);
}


BEGIN_SPECIALIZATION(_handle_event, void, Model* m, const Event& e) {
	std::cout << "Unhandled Event occured in Model\n";
} END_SPECIALIZATION;


BEGIN_SPECIALIZATION(_handle_event, void, Model* m, const CreatePlayer& e) {
	// TODO check state?
	Player* p = new Player(0, 0);
	m->players[e.ID] = p;
	m->handle_event(new Ready); // first Ready up (always synchronously)
	if (Level* l = m->get_current_level()) {
		p->pos.x = (l->width*0.125) + (((double(l->width)*0.75) / double(m->max_players-1))*e.ID) - (p->size.x/2);
		p->pos.y = l->height - Player::height_from_earth;
		m->entities.insert(p);
	} else {
		std::cout << "Model: trying to create player without a level\n";
	}
} END_SPECIALIZATION;


BEGIN_SPECIALIZATION(_handle_event, void, Model* m, const ReleasePlayer& e) {
	if (m->players.find(e.ID) != m->players.end()) {
		Player* p = m->players[e.ID];
		m->players.erase(e.ID);
		m->entities.erase(p);
		m->leftover_players.insert(e.ID);
	} else {
		std::cout << "Model: Unknown player " << e.ID << "\n";
	}
} END_SPECIALIZATION;


BEGIN_SPECIALIZATION(_handle_event, void, Model* m, const Fire& e) {
	if (m->players.find(e.ID) != m->players.end()) {
		Player* p = m->players[e.ID];
		if (p->b == nullptr) {
			// create a bullet
			Bullet* b = new Bullet(p);
			m->entities.insert(b);
			m->saved_entities.insert(b);
			// notify views that a player has shot
			m->game->notify_views(new PlayerShoots(p));
			//p->b = b;
		} else {
			std::cout << "Model: Player already has bullet\n";
		} 
	} else {
		std::cout << "Model: Unknown player " << e.ID << "\n";
	}
} END_SPECIALIZATION;


BEGIN_SPECIALIZATION(_handle_event, void, Model* m, const SetDirection& e) {
	if (m->players.find(e.ID) != m->players.end()) {
		m->players[e.ID]->mov.dir = e.dir;
	} else {
		std::cout << "Model: Unknown player " << e.ID << "\n";
	}
} END_SPECIALIZATION;


BEGIN_SPECIALIZATION(_handle_event, void, Model* m, const VCStop& e) {
	m->state = model::EXIT;
	std::cout << "Did " << m->ticks << " ticks, taking an average of " << m->avg_tick << "\n";
} END_SPECIALIZATION;


BEGIN_SPECIALIZATION(_handle_event, void, Model* m, const Ready& e) {
	// load next level
	if (m->state != model::PLAYING) {
		m->current_level++;
		if (m->current_level < m->levels.size()) {
			// level exists, go ahead
			std::cout << "Model: Loading level " << m->current_level << "\n";
			m->load_level(m->levels[m->current_level]);
			m->game->notify_all(new LevelStart(&m->levels[m->current_level]));
			m->state = model::PLAYING;
		} else {
			std::cout << "Model: Game is already over, no need to ready a new level\n";
			// level does not exist, seems some controller didn't get the message
			// broadcast again: "the game is over, stop bitching around"
			m->game->notify_views(new GameStop(m->victory));
		}
	} else {
		std::cout << "Model: Can't Ready when we're already playing\n";
		// this does however tell us that there are views/controllers out there who are not aware of our state
		m->game->notify_all(new LevelStart(m->get_current_level()));
	}
} END_SPECIALIZATION;



// === Collisions =================================================================================

// Don't forget to SYMMETRIC_SPEC(_collide, void, Model* m, Type2, Type1)

BEGIN_SPECIALIZATION(_collide, void, Model* m, Entity& a, Entity& b) {
	std::cout << "Model: Unhandled collision on " << a.pos << "\n";
} END_SPECIALIZATION;


BEGIN_SPECIALIZATION(_collide, void, Model* m, Alien& a, Bullet& b) {
	std::cout << "Model: Collision between Alien and Bullet\n";
	std::cout << "Bullet: " << b.pos << " --> " << b.pos + b.size << "\n";
	std::cout << "Alien: " << a.pos << " --> " << a.pos + a.size << "\n";
	a.killme = true;
	b.killme = true;
} END_SPECIALIZATION;

// switched
BEGIN_SPECIALIZATION(_collide, void, Model* m, Bullet& b, Alien& a) {
	GET_SPECIALIZATION(_collide, void, Model*, Alien&, Bullet&)(m, a, b);
} END_SPECIALIZATION;


BEGIN_SPECIALIZATION(_collide, void, Model* m, Bomb& b, Player& p) {
	b.killme = true;
	//p.killme = true; // TODO :P
} END_SPECIALIZATION;

BEGIN_SPECIALIZATION(_collide, void, Model* m, Player& p, Bomb& b) {
	GET_SPECIALIZATION(_collide, void, Model*, Bomb& b, Player& p)(m, b, p);
} END_SPECIALIZATION;

BEGIN_SPECIALIZATION(_collide, void, Model* m, Projectile& a, Projectile& b) {
	a.killme = true;
	b.killme = true;
} END_SPECIALIZATION;

BEGIN_SPECIALIZATION(_collide, void, Model* m, Bomb& a, Bomb& b) {
	// do nothing
} END_SPECIALIZATION;


// === Killing ====================================================================================
// return whether or not this object should be deleted

BEGIN_SPECIALIZATION(_kill, bool, Model* m, Entity& e) {
	return true; // just kill
} END_SPECIALIZATION;


BEGIN_SPECIALIZATION(_kill, bool, Model* m, Alien& e) {
	m->alien_grid[e.col][e.row] = nullptr;
	
	// check if this one is a bottom one, if so, replace with one above (unless there is none --> nullptr)
	if (m->bottom_aliens[e.col] == &e) {
		std::cout << "Model: killing bottom alien on row " << e.row << "\n";
		m->bottom_aliens[e.col] = nullptr;
		for (int row_above = e.row-1; row_above>=0; row_above--) {
			if (m->alien_grid[e.col][row_above] != nullptr) {
				m->bottom_aliens[e.col] = m->alien_grid[e.col][row_above];
				break;
			}
		}
	}
	
	// The rule of 2: If you have to do something more than twice, encapsulate it
	// This does not violate that rule :)
	
	// check if this is topleftmost one
	if (m->topleftmost == &e) {
		std::cout << "Model: killing topleftmost\n";
		m->topleftmost = nullptr;
		// search this column (top to bottom), then the one to the right, ...
		for (int col = e.col; col < m->alien_grid.size(); col++) {
			for (int row = 0; row < m->alien_grid[col].size(); row++) {
				if (m->alien_grid[col][row] != nullptr) {
					m->topleftmost = m->alien_grid[col][row];
					goto done_topleftmost;
				}
			}
		}
	}
	done_topleftmost:
	
	// check if this is toprightmost one
	if (m->toprightmost == &e) {
		std::cout << "Model: killing toprightmost\n";
		m->toprightmost = nullptr;
		// search this column (top to bottom), then the one to the left, ...
		for (int col = e.col; 0 <= col; col--) {
			for (int row = 0; row < m->alien_grid[col].size(); row++) {
				if (m->alien_grid[col][row] != nullptr) {
					m->toprightmost = m->alien_grid[col][row];
					goto done_toprightmost;
				}
			}
		}
	}
	done_toprightmost:
	
	m->aliens_alive--;
	if (m->aliens_alive == 0) m->win_level();
	
	return true;
} END_SPECIALIZATION;



}
}
