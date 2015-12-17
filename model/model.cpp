
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
	
	for (int i=0; i<max_players; i++) {
		leftover_players.load().insert(i);
	}
}

std::vector<std::thread*> Model::start() {
	// TODO assert only one thread
	return {new std::thread(&Model::loop, this)};
}

void Model::loop() {
	// The game has started!
	game->notifyViews(new GameStart);
	game->model_lock();
	util::Stopwatch::TimePoint current_tick = watch.now();
	util::Stopwatch::TimePoint prev_tick;
	
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
			handleEvent(e);
		}
		game->entity_lock.write_unlock();
		
		// update views
		// Could be concurrent, could be blocking. The View is responsible for that.
		game->notifyViews(new Tick);
	}
	game->model_unlock();
	game->notifyViews(new GameStop);
}

EntityRange Model::all_entities() const {
	return EntityRange(*this);
}

model::Player* Model::get_player() const {
	return player;
}


