
#include "model.hpp"
#include "game/game.hpp"
#include "exceptions/exceptions.hpp"


using namespace si;
using namespace si::model;
using namespace si::util;

// Level
Level::Level(const picojson::object& conf) {
	name = get<std::string>(conf, "name");
	width = get<unsigned int>(conf, "width");
	height = get<unsigned int>(conf, "height");
	alien_rows = get<unsigned int>(conf, "alien_rows");
	alien_cols = get<unsigned int>(conf, "alien_cols");
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
		max_players = get<unsigned int>(m, "max_players");
		if (m.find("levels")->second.is<picojson::array>()) {
			for (const auto& l: m.find("levels")->second.get<picojson::array>()) {
				levels.push_back(Level(convert<picojson::object>(l)));
			}
		} else {
			throw ParseError("levels should be an array");
		}
	} else {
		throw ParseError("Root should be an object");
	}
	
	for (unsigned int i=0; i<max_players; i++) {
		leftover_players.insert(i);
	}
}

std::vector<std::thread*> Model::start() {
	// TODO assert only one thread
	return {new std::thread(&Model::loop, this)};
}

void Model::loop() {
	// The game has started!
	game->notifyViews(new ModelStart);
	game->notifyControllers(new ModelStart);
	
	while (true) {
		switch (state) {
			case State::WAIT:
				Wait();
				break;
			case State::PLAYING:
				Playing();
				break;
			case State::RECAP:
				Recap();
				break;
			case State::EXIT:
				return;
		}
	}
}

void Model::Playing() {
	util::Stopwatch::TimePoint current_tick = watch.now();
	util::Stopwatch::TimePoint prev_tick;
	
	while (state == PLAYING) {
		// tick
		game->entity_lock.write_lock();
		prev_tick = current_tick;
		current_tick = watch.now();
		util::Stopwatch::Duration duration = current_tick - prev_tick;
		
		for (Entity* e: entities) {
			e->mov.perform(duration, e->pos);
		}
		
		// handle Events, until nullptr
		while (Event* e = game->get_controller_event()) {
			handleEvent(e);
		}
		game->entity_lock.write_unlock();
		
		// update views
		// Could be concurrent, could be blocking. The View is responsible for that.
		game->notifyViews(new Tick);
	}
}

void Model::loadLevel(Level& l) {
	unloadLevel();
	// add players
	for (auto& it: players) {
		entities.insert(it.second.get());
	}
	// create aliens TODO
	saved_entities.insert(new Alien(350, 350));
	for (auto& e: saved_entities) {
		entities.insert(e);
	}
}

void Model::unloadLevel() {
	// not players
	for (Entity* e: saved_entities) {
		delete e;
	}
	saved_entities.clear();
	entities.clear();
}


void Model::Wait() {
	while (Event* e = game->get_controller_event()) {
		std::cout << "Model in Wait, got event!\n";
		handleEvent(e);
	}
}

void Model::Recap() {
	
}


EntityRange Model::all_entities() const {
	return EntityRange(*this);
}



