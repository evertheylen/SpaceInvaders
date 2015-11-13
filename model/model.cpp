
#include "model.hpp"
#include "game/game.hpp"

using namespace si;
using namespace si::model;

Model::Model(const TiXmlDocument& doc, Game* g): game(g) {
	player = new model::Player;
	player->x = 200;
	player->y = 550;
	entities.insert(std::unique_ptr<Entity>(player));
}

void Model::handleEvent(si::Event* e) {
	std::cout << "Model got Event\n";
	// TODO DISPATCHERS LOL NOT THIS EASY BULLSHIT
	// THIS IS C++, NOT PYTHON ROFL
	switch (e->type()) {
		case EventType::START_MOVE: {
			StartMove* ee = static_cast<StartMove*>(e);
			si::util::perform(ee->dir, ee->player->x, ee->player->y, 1.0);
			std::cout << "y: " << ee->player->y << "\n";
			break;
		} default: {
			break;
		}
	}
	
	// hey Views, fucking redraw
	// TODO: memory leaks
	game->notifyViews(new Redraw);
}

model::Player* Model::get_player() const {
	return player;
}


