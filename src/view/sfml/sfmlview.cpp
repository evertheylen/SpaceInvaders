
#include "sfmlview.hpp"
#include "game/game.hpp"
#include "util/util.hpp"
#include "controller/sfml/sfmlcontroller.hpp"

#include <string>

using namespace si;
using namespace si::view;


SfmlView::SfmlView(Game* g, bool _concurrent): game(g), concurrent(_concurrent) {
	// nothing?
}


std::vector<std::thread*> SfmlView::start() {
	assert(handle != nullptr);
	std::cout << "View has started\n";
	if (is_concurrent()) {
		std::vector<std::thread*> v = handle->start();
		return v + std::vector<std::thread*>{new std::thread(&SfmlView::loop, this)};
	} else {
		return {};
	}
}

void SfmlView::resize(int width, int height, bool blur) {
	if (blur) backgroundsprite = sf::Sprite(res.background_blur);
	else backgroundsprite = sf::Sprite(res.background);
	
	sf::View vw(sf::FloatRect(0.0, 0.0, width, height));
	vw.setCenter(double(width)/2, double(height)/2);
	handle->window->setView(vw);
	handle->window->setSize(sf::Vector2u(width, height));
	
	backgroundsprite.setScale(double(width) / 800, double(height) / 600);
	backgroundsprite.setPosition(0, 0);
	handle->window->clear();
	handle->window->display();
}


void SfmlView::redraw() {
	handle->window->draw(backgroundsprite);
	
	for (const model::Entity* e: game->get_model().all_entities()) {
		_draw(this, *e);
	}
}

void SfmlView::simple_draw(sf::Texture& tex, const model::Entity& e) {
	sf::RectangleShape s(sf::Vector2f(e.size.x, e.size.y));
	s.setTexture(&tex);
	//s.setTextureRect(sf::IntRect(e.pos.x, e.pos.y, e.size.x, e.size.y));
	s.setPosition(e.pos.x, e.pos.y);
	handle->window->draw(s);
}


void SfmlView::handle_event(Event* e) {
	std::cerr << " [ V<  M   C ] SfmlView received: " << e->name() << "\n";
	if (phase == model::EXIT) return;
	if (is_concurrent()) {
		// no ticks are being sent if we're concurrent
		queue.push(e);
	} else {
		queue.push(e);
		std::cout << "SfmlView: pushed on queue\n";
		switch (phase) {
			case model::PRE_WAIT:
				handle_all_events(); break;
			case model::WAIT:
				wait(); break;
			case model::PLAYING:
				playing_syn(); break;
			case model::RECAP:
				recap(); break;
			case model::GAMEOVER:
				gameover(); break;
			default:
				break;
		}
		std::cout << "SfmlView: after switch\n";
	}
}


void SfmlView::loop() {
	while (true) {
		switch (phase) {
			case model::PRE_WAIT:
				while (phase == model::PRE_WAIT) handle_all_events();
				break;
			case model::WAIT:
				while (phase == model::WAIT) wait();
				break;
			case model::PLAYING:
				while (phase == model::PLAYING) playing_cc();
				break;
			case model::RECAP:
				while (phase == model::RECAP) recap();
				break;
			case model::GAMEOVER:
				while (phase == model::GAMEOVER) gameover();
				break;
			case model::EXIT:
				return;
		}
	}
}

void SfmlView::phase_change(model::Phase newphase) {
	switch (newphase) {
		case model::WAIT: {
			std::cout << "SfmlView: Model should start\n";
			if (not concurrent and handle->controller == nullptr)
				handle->init();
			handle->window->setActive(true);
			handle->window->clear(sf::Color::Black);
			handle->window->display();
			std::cout << "SfmlView: Model should start\n";
		} break;
		case model::PLAYING: {
			std::cout << "SfmlView starts playing\n";
			model::Level* level = model_state.playing.level;
			resize(level->width, level->height, false);
		} break;
		case model::RECAP: {
			resize(800, 600, true);
			handle->window->clear();
			handle->window->draw(backgroundsprite);
			draw_text(50, 50, "Recap! scores go here");
			handle->window->display();
		} break;
		case model::GAMEOVER: {
			resize(800, 600, true);
			handle->window->clear();
			handle->window->draw(backgroundsprite);
			draw_text(100, 100, "Game Over!");
			handle->window->display();
		} break;
		default:
			break;
	}
	phase = newphase;
}

void SfmlView::draw_text(int x, int y, const std::__cxx11::string& s) {
	sf::Text text;
	text.setFont(res.font);
	text.setString(s);
	text.setCharacterSize(24); // in pixels, not points!
	text.setColor(sf::Color::Green);
	text.setPosition(x, y);
	handle->window->draw(text);
}


void SfmlView::handle_all_events() {
	while (not queue.empty()) {
		Event* e = queue.pop();
		_handle_event(this, *e);
		delete e;
	}
}

void SfmlView::draw_overlay() {
	// if the controller isn't started yet, we should always display such a message
	if (handle->controller != nullptr && handle->controller->get_player() < 0) {
		sf::RectangleShape s(sf::Vector2f(handle->window->getSize().x, handle->window->getSize().y));
		s.setFillColor(sf::Color(0,0,0,125));
		s.setPosition(0, 0);
		handle->window->draw(s);
		
		draw_text(50, 100, "Press Space to start playing");
	}
}

void SfmlView::draw_score() {
	// draw score and lives
	if (handle->controller != nullptr && handle->controller->get_player() >= 0) {
		model::Player* p = game->get_model().get_player(handle->controller->get_player());
		if (p != nullptr) {
			draw_text(10, 10, std::string("score: ") + std::to_string(p->score) + ", lives: " + std::to_string(p->lives));
		} else {
			std::cout << "SfmlView: no score because nullptr\n";
		}
	} else {
		std::cout << "SfmlView: no score because other reason\n";
	}
}


void SfmlView::wait() {
	handle->window->clear();
	handle_all_events();
	handle->window->draw(backgroundsprite);
	draw_overlay();
	handle->window->display();
}

void SfmlView::playing_cc() {
	timestamp = game->entity_lock.new_timestamp();
	if (game->entity_lock.read_lock(timestamp)) {
		handle->window->clear();
		redraw(); // always 'expect' a tick
			
		// stats
		if (ticks%50001 == 0) {
			std::cout << "SfmlView: Did 50000 ticks\n";
			ticks = 1;
		} else {
			ticks++;
		}
		
		handle_all_events();
		draw_score();
		game->entity_lock.read_unlock(timestamp);
		
		draw_overlay();
		handle->window->display();
	} else {
		std::cout << "SfmlView: read_lock has been closed\n";
		return;
	}
}

void SfmlView::playing_syn() {
	handle->window->clear();
	redraw(); // always 'expect' a tick
	
	// stats
	if (ticks%50001 == 0) {
		std::cout << "SfmlView: Did 50000 ticks\n";
		ticks = 1;
	} else {
		ticks++;
	}
	
	handle_all_events();
	draw_score();
	draw_overlay();
	handle->window->display();
}

void SfmlView::recap() {
	handle->window->clear();
	handle_all_events();
	handle->window->draw(backgroundsprite);
	draw_text(50, 50, "Recap! scores go here");
	draw_overlay();
	handle->window->display();
}

void SfmlView::gameover() {
	handle->window->clear();
	handle_all_events();
	handle->window->draw(backgroundsprite);
	draw_text(50, 50, "Game Over");
	draw_overlay();
	handle->window->display();
}



