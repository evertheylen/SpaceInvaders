
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
	// It's important I notify the controllers first
	// The controller needs to start the window
	game->notify_controllers(new ModelStart);
	game->notify_views(new ModelStart);
	
	while (true) {
		switch (state) {
			case State::WAIT:
				wait();
				break;
			case State::PLAYING:
				playing();
				break;
			case State::RECAP:
				recap();
				break;
			case State::EXIT:
				return;
		}
	}
}

void Model::playing() {
	util::Stopwatch::TimePoint current_tick = watch.now();
	util::Stopwatch::TimePoint prev_tick;
	
	while (state == PLAYING) {
		// tick
		prev_tick = current_tick;
		current_tick = watch.now();
		util::Stopwatch::Duration duration = current_tick - prev_tick;
		
		// statistics
		if (ticks%100001 == 0) {
			std::cout << "Model: Did 100000 ticks, avg: " << avg_tick << "\n";
			ticks = 1;
			avg_tick = duration.count();
		} else {
			avg_tick += (duration.count() - avg_tick)/double(ticks);
			ticks++;
		}
		
		game->entity_lock.write_lock();
		
		for (Entity* e: entities) {
			e->mov.perform(duration, e->pos);
		}
		
		// handle Events, until nullptr
		while (Event* e = game->get_input_event()) {
			handle_event(e);
		}
		game->entity_lock.write_unlock();
		
		// update views
		// Could be concurrent, could be blocking. The View is responsible for that.
		game->notify_views(new Tick);
	}
}

void Model::wait() {
	while (Event* e = game->get_input_event()) {
		std::cerr << " [ V   M<--C ] Model pulling Event from Controllers: " << e->name() << "\n";
		handle_event(e);
	}
}

void Model::recap() {
	
}

void Model::load_level(Level& l) {
	unload_level();
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

void Model::unload_level() {
	// not players
	for (Entity* e: saved_entities) {
		delete e;
	}
	saved_entities.clear();
	entities.clear();
}


EntityRange Model::all_entities() const {
	return EntityRange(*this);
}



