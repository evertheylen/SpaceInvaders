
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


BEGIN_SPECIALIZATION(_handle_event, void, Model* m, const AskPlayer& e) {
	int pl = -1;
	for (unsigned int i: m->leftover_players) {
		m->leftover_players.erase(i);
		pl = i;
		break;
	}
	if (pl < 0) {
		std::cout << "Model: no more players available\n";
		// TODO send message back "sorry mate"
		return;
	} else {
		e.obv->handle_event(new CreatePlayer(pl));
		m->players[pl] = new Player(0, 0);  // load_level will reset this to a proper position
		if (m->current_level < m->levels.size()) {
			m->place_player(m->levels[m->current_level], pl, m->players[pl]);
		}
	}
		
	if (m->state.phase() == WAIT) {
		// someone expects us to plaaaay
		m->handle_event(new Ready); // first Ready up (always synchronously)
	}
} END_SPECIALIZATION;


BEGIN_SPECIALIZATION(_handle_event, void, Model* m, const ReleasePlayer& e) {
	if (m->players.find(e.ID) != m->players.end()) {
		Player* p = m->players[e.ID];
		m->players.erase(e.ID);
		m->entities.erase(p);
		m->leftover_players.insert(e.ID); // TODO
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
			p->b = b;
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
	m->state.set(model::EXIT);
	std::cout << "Did " << m->ticks << " ticks, taking an average of " << m->avg_tick << "\n";
} END_SPECIALIZATION;


BEGIN_SPECIALIZATION(_handle_event, void, Model* m, const Ready& e) {
	if (m->state.phase() == model::GAMEOVER) return;
	// load next level
	if (m->state.phase() != model::PLAYING) {
		m->current_level++;
		if (m->current_level < m->levels.size()) {
			// level exists, go ahead
			std::cout << "Model: Loading level " << m->current_level << "\n";
			m->load_level(m->levels[m->current_level]);
			m->state.set(model::PLAYING);
		} else {
			m->state.set(model::GAMEOVER);
		}
	} else {
		std::cout << "Model: Can't Ready when we're already playing\n";
	}
} END_SPECIALIZATION;



// === Collisions =================================================================================


BEGIN_SPECIALIZATION(_collide, void, Model* m, Entity& a, Entity& b) {
	std::cout << "Model: Unhandled collision on " << a.pos << "\n";
} END_SPECIALIZATION;


BEGIN_SPECIALIZATION(_collide, void, Model* m, Alien& a, Bullet& b) {
	std::cout << "Model: Collision between Alien and Bullet\n";
	std::cout << "Bullet: " << b.pos << " --> " << b.pos + b.size << "\n";
	std::cout << "Alien: " << a.pos << " --> " << a.pos + a.size << "\n";
	a.killme = true;
	b.killme = true;
	b.p->score += 10;
} END_SPECIALIZATION;

// switched
BEGIN_SPECIALIZATION(_collide, void, Model* m, Bullet& b, Alien& a) {
	GET_SPECIALIZATION(_collide, void, Model*, Alien&, Bullet&)(m, a, b);
} END_SPECIALIZATION;

BEGIN_SPECIALIZATION(_collide, void, Model* m, Alien& a, Earth& e) {
	std::cout << "Model: Collision between Alien and Earth\n";
	m->lose_level();
} END_SPECIALIZATION;

// switched
BEGIN_SPECIALIZATION(_collide, void, Model* m, Earth& e, Alien& a) {
	GET_SPECIALIZATION(_collide, void, Model*, Alien&, Earth&)(m, a, e);
} END_SPECIALIZATION;


BEGIN_SPECIALIZATION(_collide, void, Model* m, Bomb& b, Player& p) {
	b.killme = true;
	p.killme = true;
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

BEGIN_SPECIALIZATION(_kill, bool, Model* m, Bullet& b) {
	b.p->b = nullptr;
	return true;
} END_SPECIALIZATION;

BEGIN_SPECIALIZATION(_kill, bool, Model* m, Player& p) {
	if (p.lives <= 1) {
		p.lives = 0;
		m->players_alive--;
		if (m->players_alive <= 0) m->lose_level();
		return true;
	} else {
		p.lives--;
		return false;
	}
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
