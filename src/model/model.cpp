
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
	alien_speed = get<double>(conf, "alien_speed");
	// make sure there is enough space
	if ((alien_cols+1)*(Alien::gap_x) - Alien::width >= width) {
		throw ParseError("not enough room (in x direction)\n");
	}
	if (Player::height_from_earth + (alien_rows+1)*(Alien::gap_y) - Alien::height >= height) {
		throw ParseError("not enough room (in y direction)\n");
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
		max_players = get<unsigned int>(m, "max_players");
		if (m.find("levels")->second.is<picojson::array>()) {
			std::cout << conf.to_str() << "\n";
			for (const auto& l: m.find("levels")->second.get<picojson::array>()) {
				std::cout << "Model: reading a level\n";
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
	
	alien_periodical = util::Periodical({12}); // TODO
}

// make sure you have read_lock before calling
Player* Model::get_player(int ID) const {
	if (players.find(ID) != players.end()) {
		return players.find(ID)->second;
	} else {
		return nullptr;
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
	state.set(WAIT);
	
	while (true) {
		// first prepare state data
		switch (state.phase()) {
			case PLAYING:
				state.playing.level = &levels[current_level];
				state.playing.levelnum = current_level;
				break;
			case RECAP:
				state.playing.level = &levels[current_level];
				state.recap.levelnum = current_level;
				state.recap.scores[0] = 500; // TODO
				break;
			case GAMEOVER:
				state.gameover.victory = victory; // TODO
				break;
			default:
				break;
		}
		
		// then notify all of the state change
		game->notify_all(new ModelStateChange(state));
		
		// then select the right loop
		switch (state.phase()) {
			case PRE_WAIT:
			case WAIT:
				wait();
				break;
			case PLAYING:
				playing();
				break;
			case RECAP:
				recap();
				break;
			case GAMEOVER:
				gameover();
				break;
			case EXIT:
				return; // and close this thread
		}
	}
}

void Model::playing() {
	bool wait = game->has_cc_viewers();
	util::Stopwatch::TimePoint current_tick = watch.now();
	util::Stopwatch::TimePoint prev_tick;
	
	int move_alien_row = 0;
	int move_alien_col = 0;
	
	while (state.phase() == PLAYING) {
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
		
		// update views
		// Could be concurrent, could be blocking. The View is responsible for that.
		game->notify_views(new Tick);
		std::cout << "Model: after tick\n";
		
		game->entity_lock.write_lock();
		std::cout << "Model: after write lock\n";
		
		//BEGIN Actual calculations of next 'frame' ---
		// movemements
		for (Entity* e: saved_entities) {
			if (e->mov.dir.length() > 0) {
				e->mov.perform(duration, e->pos);
				check_collisions(e);
			}
			if (not check_collision(e, &world)) {
				e->killme = true; // out of world
			}
		}
		
		for (auto iter = players.begin(); iter != players.end(); iter++) {
			Player* e = iter->second;
			if (e->mov.dir.length() > 0) {
				e->mov.perform(duration, e->pos);
				check_collisions(e);
			}
			if (e->pos.x <= 0.0) {
				e->pos.x = 0.0;
			} else if ((e->pos.x + e->size.x) >= world.size.x) {
				e->pos.x = world.size.x;
			}
		}
		
		// handle Events, until nullptr
		while (Event* e = game->get_input_event()) {
			handle_event(e);
		}
		
		// move an Alien, let them shoot
		if (alien_periodical.advance(duration)) {
			move_alien(move_alien_row, move_alien_col);
			if (not next_alien(move_alien_row, move_alien_col)) {
				// start again from above
				move_alien_row = 0;
				move_alien_col = 0;
				if (alien_grid[move_alien_col][move_alien_row] == nullptr) {
					next_alien(move_alien_row, move_alien_col);
				}
				if (alien_mov_state == DOWNLEFT) {
					alien_mov_state = LEFT;
				}
				if (alien_mov_state == DOWNRIGHT) {
					alien_mov_state = RIGHT;
				}
			}
			for (Alien* a: bottom_aliens) {
				if (a != nullptr and util::RandomGenerator::instance()->flip(0.025/aliens_alive)) { // TODO
					std::cout << "Model: Pew\n";
					Bomb* b = new Bomb(a);
					saved_entities.insert(b);
					entities.insert(b);
				}
			}
		}
		
		// delete entities
		// Again, this would be easier with generators
		for (auto iter = saved_entities.begin(); iter != saved_entities.end(); ) {
			if ((*iter)->killme) {
				if (_kill(this, *(*iter))) {
					delete *iter;
					entities.erase(*iter);
					saved_entities.erase(iter++);
				} else {
					(*iter)->killme = false;
				}
			} else {
				++iter;
			}
		}
		
		for (auto iter = players.begin(); iter != players.end(); ) {
			if (iter->second->killme) {
				if (_kill(this, *(iter->second))) {
					entities.erase(iter->second);
					iter++;
				} else {
					iter->second->killme = false;
				}
			} else {
				++iter;
			}
		}
		//END
		
		game->entity_lock.write_unlock();
		
		// BUG this is pthread's fault. While there are readers waiting, it does not give them a chance,
		// although it is set to PREFER_READER_NP
		if (wait) {
			std::this_thread::sleep_for(std::chrono::nanoseconds(5000));
		}
	}
}

void Model::wait() {
	while (state.phase() == WAIT) {
		while (Event* e = game->get_input_event()) {
			std::cerr << " [ V   M<--C ] Model pulling Event from Controllers: " << e->name() << "\n";
			handle_event(e);
		}
		game->notify_views(new Tick);
	}
}

void Model::recap() {
	while (state.phase() == RECAP) {
		while (Event* e = game->get_input_event()) {
			std::cerr << " [ V   M<--C ] Model pulling Event from Controllers: " << e->name() << "\n";
			handle_event(e);
		}
		game->notify_views(new Tick);
	}
}

void Model::gameover() {
	while (state.phase() == GAMEOVER) {
		while (Event* e = game->get_input_event()) {
			std::cerr << " [ V   M<--C ] Model pulling Event from Controllers: " << e->name() << "\n";
			handle_event(e);
		}
		game->notify_views(new Tick);
	}
}


// note: assumes a is the smaller object
bool Model::check_collision(Entity* a, Entity* b) {
	double xmin = b->pos.x;
	double xmax = b->pos.x + b->size.x;
	double ymin = b->pos.y;
	double ymax = b->pos.y + b->size.y;
	// is there a point of a (corners) within b?
	#define check_in_b(point_x, point_y) if ((xmin <= point_x and point_x <= xmax) and (ymin <= point_y and point_y <= ymax)) return true
	check_in_b(a->pos.x, a->pos.y);
	check_in_b(a->pos.x + a->size.x, a->pos.y);
	check_in_b(a->pos.x, a->pos.y + a->size.y);
	check_in_b(a->pos.x + a->size.x, a->pos.y + a->size.y);
	return false;
	#undef check_in_b
}


void Model::check_collisions(Entity* a) {
	for (Entity* b: entities) {
		if (a != b) {
			if (check_collision(a, b) or check_collision(b, a)) {
				_collide(this, *a, *b);
			}
		}
	}
}


void Model::move_alien(int& row, int& col) {
	if (topleftmost == nullptr && toprightmost == nullptr) return;
	
	// first, select an alien
	Alien* a = alien_grid[col][row];
	if (a == nullptr) return;
	
	// set up movement
	util::Vector2D_d mov(8, 0);
	if (alien_mov_state == LEFT) mov *= -1;
	
	// calculations for out of world
	double left = a->pos.x - ((col - topleftmost->col) * Alien::gap_x);
	double right = (toprightmost->col - col) * Alien::gap_x + Alien::width + a->pos.x;
	
	// check if we'd be out of the world
	if (alien_mov_state == LEFT) {
		if (left + mov.x <= 10.0) {
			alien_mov_state = DOWNRIGHT;
		}
	} else {
		if (right + mov.x >= world.size.x - 10.0) {
			alien_mov_state = DOWNLEFT;
		}
	}
	
	// check if we're going down
	if (alien_mov_state == DOWNLEFT or alien_mov_state == DOWNRIGHT) {
		mov = util::Vector2D_d(0, 12);
	}
	
	// perform the move
	a->pos += mov;
	check_collisions(a);
}


bool next(int& row, int& col, int max_rows, int max_cols) {
	// assuming current position is legit
	col++;
	if (col >= max_cols) {
		col = 0;
		row++;
	}
	if (row >= max_rows) {
		return false;
	}
	return true;
}


bool Model::next_alien(int& row, int& col) {
	int max_cols = levels[current_level].alien_cols;
	int max_rows = levels[current_level].alien_rows;
	
	while (next(row, col, max_rows, max_cols)) {
		if (alien_grid[col][row] != nullptr) {
			return true;
		}
	}
	return false;
}

void Model::win_level() {
	int next_level = current_level + 1;
	if (next_level > levels.size()) {
		victory = true;
	}
	state.set(RECAP);
}

void Model::lose_level() {
	victory = false;
	state.set(GAMEOVER);
}


void Model::load_level(Level& l) {
	unload_level();
	// set world entity for out-of-bounds checking
	world = Entity(0,0);
	world.size.x = l.width;
	world.size.y = l.height;
	
	// also add 'earth': flat entity at the bottom
	Earth* earth = new Earth(0,l.height-1);
	earth->size.x = l.width;
	earth->size.y = 1;
	saved_entities.insert(earth);
	entities.insert(earth);
	
	// add players
	for (auto& it: players) {
		if (it.second->lives > 0) place_player(l, it.first, it.second);
	}
	
	// create aliens
	bottom_aliens.assign(l.alien_cols, nullptr);
	alien_grid.assign(l.alien_cols, {});
	for (auto& row: alien_grid) row.assign(l.alien_rows, nullptr);
	double x = Alien::gap_x - Alien::width;
	for (int col=0; col<l.alien_cols; col++) {
		double y = Alien::gap_y - Alien::height;
		for (int row=0; row<l.alien_rows; row++) {
			Alien* a = new Alien(x, y, col, row);
			alien_grid[col][row] = a;
			saved_entities.insert(a);
			entities.insert(a);
			y += Alien::gap_y;
		}
		bottom_aliens.at(col) = alien_grid[col][l.alien_rows-1];
		x += Alien::gap_x;
	}
	topleftmost = alien_grid[0][0];
	toprightmost = alien_grid[l.alien_cols-1][0];
	aliens_alive = l.alien_cols * l.alien_rows;
	
	for (auto& e: saved_entities) {
		entities.insert(e);
	}
}

void Model::place_player(Level& l, int ID, Player* p) {
	entities.insert(p);
	p->pos.x = (l.width*0.125) + (((double(l.width)*0.75) / double(max_players-1))*ID) - (p->size.x/2);
	p->pos.y = l.height - Player::height_from_earth;
	players_alive++;
}


void Model::unload_level() {
	players_alive = 0;
	// not players
	for (Entity* e: saved_entities) {
		delete e;
	}
	for (auto it: players) {
		Player* p = it.second;
		p->mov.dir *= 0;
	}
	alien_grid.clear();
	bottom_aliens.clear();
	topleftmost = nullptr;
	toprightmost = nullptr;
	saved_entities.clear();
	entities.clear();
}


EntityRange Model::all_entities() const {
	return EntityRange(*this);
}



