
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



// EntityRange

EntityRange::EntityRange(const Model& _model):
		model(_model) {}

typename EntityRange::CollectionT::const_iterator EntityRange::begin() {
	return model.entities.begin();
}

typename EntityRange::CollectionT::const_iterator EntityRange::end() {
	return model.entities.end();
}

unsigned int EntityRange::size() {
	return model.entities.size();
}



// Model

Model::Model(const picojson::value& conf, Game* g): game(g) {
	if (conf.is<picojson::object>()) {
		picojson::object m = conf.get<picojson::object>();
		if (m.find("levels")->second.is<picojson::array>()) {
			for (const auto& l: m.find("levels")->second.get<picojson::array>()) {
				Level somelevel(l); // TODO use this :)
			}
		} else {
			throw ParseError("levels should be an array");
		}
	} else {
		throw ParseError("Root should be an object");
	}
	
	player = new model::Player(200, 550);
	player->mov.speed = 0.2; // TODO
	entities.push_back(std::unique_ptr<Entity>(player));
}

std::vector<std::thread*> Model::start() {
	// TODO assert only one thread
	return {new std::thread(&Model::loop, this)};
}

void Model::loop() {
	game->model_lock();
	util::Stopwatch::TimePoint current_tick = watch.now();
	util::Stopwatch::TimePoint prev_tick;
	
	int changed_pos = 0;
	
	while (true) {
		// TODO handle exit --> break
		
		// tick
		game->entity_lock.write_lock();
		prev_tick = current_tick;
		current_tick = watch.now();
		util::Stopwatch::Duration duration = current_tick - prev_tick;
		
		for (const std::unique_ptr<Entity>& e: entities) {
			e->mov.perform(duration, e->pos);
		}
		
		// handle Events, until nullptr
		while (Event* e = game->get_controller_event()) {
			if (SetMovement* m = dynamic_cast<SetMovement*>(e)) {
				m->entity->mov.dir = m->dir;
				std::cout << "setting dir to " << m->entity->mov.dir;
				changed_pos = 0;
			}
			handleEvent(e);
		}
		game->entity_lock.write_unlock();
		
		// update views
		// Could be concurrent, could be blocking. The View is responsible for that.
		game->notifyViews(new Tick);
	}
	game->model_unlock();
}

EntityRange Model::all_entities() const {
	return EntityRange(*this);
}

model::Player* Model::get_player() const {
	return player;
}


