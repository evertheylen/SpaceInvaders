
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
	m->players[e.ID] = std::move(std::unique_ptr<Player>(p));
	if (m->state == model::WAIT) {
		m->handle_event(new Ready);
	}
} END_SPECIALIZATION;


BEGIN_SPECIALIZATION(_handle_event, void, Model* m, const ReleasePlayer& e) {
	Player* p = m->players[e.ID].get();
	m->players.erase(e.ID);
	m->entities.erase(p);
	m->leftover_players.insert(e.ID);
} END_SPECIALIZATION;


BEGIN_SPECIALIZATION(_handle_event, void, Model* m, const Fire& e) {
	Player* p = m->players[e.ID].get();
	// create a bullet
	Bullet* b = new Bullet(p->pos.x+10, p->pos.y+8); // TODO :)
	m->entities.insert(b);
	m->saved_entities.insert(b);
	// notify views that a player has shot
	m->game->notify_views(new PlayerShoots(p));
} END_SPECIALIZATION;


BEGIN_SPECIALIZATION(_handle_event, void, Model* m, const SetDirection& e) {
	m->players[e.ID]->mov.dir = e.dir;
} END_SPECIALIZATION;


BEGIN_SPECIALIZATION(_handle_event, void, Model* m, const VCStop& e) {
	m->state = model::EXIT;
	std::cout << "Did " << m->ticks << " ticks, taking an average of " << m->avg_tick << "\n";
} END_SPECIALIZATION;

BEGIN_SPECIALIZATION(_handle_event, void, Model* m, const Ready& e) {
	// load next level
	m->current_level++;
	if (m->current_level < m->levels.size()) {
		// level exists, go ahead
		std::cout << "Model: Loading level " << m->current_level << "\n";
		m->load_level(m->levels[m->current_level]);
		m->game->notify_views(new LevelStart);
		m->game->notify_controllers(new LevelStart);
		m->state = model::PLAYING;
	} else {
		std::cout << "Model: Game is already over, no need to ready a new level\n";
		// level does not exist, seems some controller didn't get the message
		// broadcast again: "the game is over, stop bitching around"
		m->game->notify_views(new GameStop(m->victory));
	}
} END_SPECIALIZATION;



}
}
