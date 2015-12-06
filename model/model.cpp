
#include "model.hpp"
#include "game/game.hpp"
#include "exceptions/exceptions.hpp"

using namespace si;
using namespace si::model;

// Level
Level::Level(const picojson::value& conf) {
	if (conf.is<picojson::object>()) {
		picojson::object m = conf.get<picojson::object>();
		if (m.find("name") == m.end() or m.find("players") == m.end() or m.find("aliens") == m.end()) {
			throw ParseError("Level did not contain the necessary attributes.");
		}
		if (not m.find("name")->second.is<std::string>()) throw ParseError("'name' of level is not a string");
		name = m.find("name")->second.get<std::string>();
		if (not m.find("players")->second.is<double>()) throw ParseError("'players' of level is not an int");
		players = m.find("players")->second.get<double>();
		if (not (players > 0)) throw ParseError("'players' should be > 0");
		if (not m.find("aliens")->second.is<bool>()) throw ParseError("'aliens' of level is not a bool");
		aliens = m.find("aliens")->second.get<bool>();
	} else {
		throw ParseError("Level is not an object");
	}
}



Model::Model(const picojson::value& conf, Game* g): game(g) {
	player = new model::Player(200, 550);
	player->mov.speed = 0.1; // TODO
	entities.insert(std::unique_ptr<Entity>(player));
}

std::thread* Model::start() {
	// TODO assert only one thread
	return new std::thread(&Model::loop, this);
}

void Model::loop() {
	game->model_lock();
	util::Stopwatch::TimePoint current_tick = watch.now();
	util::Stopwatch::TimePoint prev_tick;
	while (true) {
		// TODO handle exit --> break
		
		// tick
		prev_tick = current_tick;
		current_tick = watch.now();
		auto duration = current_tick - prev_tick;
		for (const std::unique_ptr<Entity>& e: entities) {
			// TODO movements...
			e->mov.perform(duration, e->pos);
		}
		
		// handle Events, until nullptr
		while (Event* e = game->get_controller_event()) {
			if (SetMovement* m = dynamic_cast<SetMovement*>(e)) {
				m->entity->mov.dir = m->dir;
				std::cout << "setting dir\n";
			}
		}
		
		// update views
		game->notifyViews(new Redraw);
	}
	game->model_unlock();
}


model::Player* Model::get_player() const {
	return player;
}


