
#include <cassert>

#include "yorel/multi_methods.hpp"

using yorel::multi_methods::virtual_;

#include "model.hpp"
#include "game/game.hpp"

using namespace si;
using namespace si::model;

namespace si {
namespace model {


void Model::handle_event(Event* e) {
	_handle_event(this, *e);
}


BEGIN_SPECIALIZATION(_handle_event, void, Model* m, const Event& e) {
	std::cout << "Unhandled Event occured in Model\n";
} END_SPECIALIZATION;


BEGIN_SPECIALIZATION(_handle_event, void, Model* m, const CreatePlayer& e) {
	Player* p = new Player(200, 550); // TODO level
	p->mov.speed = 0.2;
	m->entities.insert(p);
	m->players[e.ID] = p;
	m->handle_event(new Ready);
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
			m->game->notify_all(new LevelStart);
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
		m->game->notify_all(new LevelStart);
	}
} END_SPECIALIZATION;



// Collisions

// Don't forget to SYMMETRIC_SPEC(_collide, void, Model* m, Type2, Type1)

BEGIN_SPECIALIZATION(_collide, void, Model* m, Entity& a, Entity& b) {
	std::cout << "Model: Unhandled collision on " << a.pos << "\n";
} END_SPECIALIZATION;


BEGIN_SPECIALIZATION(_collide, void, Model* m, Alien& a, Bullet& b) {
	a.killme = true;
	b.killme = true;
} END_SPECIALIZATION;

// switched
BEGIN_SPECIALIZATION(_collide, void, Model* m, Bullet& b, Alien& a) {
	GET_SPECIALIZATION(_collide, void, Model*, Alien&, Bullet&)(m, a, b);
} END_SPECIALIZATION


}
}
