
#include "sfmlview.hpp"
#include "game/game.hpp"
#include "util/util.hpp"

using namespace si;
using namespace si::view;


SfmlView::SfmlView(Game* g, bool _concurrent): game(g), concurrent(_concurrent) {
	backgroundsprite.setTextureRect(sf::IntRect(0,0,800,600));
	backgroundsprite.setTexture(res.background);
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


void SfmlView::redraw() {
	handle->window->draw(backgroundsprite);
	
	for (const model::Entity* e: game->get_model().all_entities()) {
		_draw(this, *e);
	}
}

void SfmlView::simple_draw(sf::Texture& tex, const model::Entity& e) {
	sf::Sprite s;
	s.setTexture(tex);
	//s.setTextureRect(sf::IntRect(e.pos.x, e.pos.y, e.size.x, e.size.y));
	s.setPosition(e.pos.x, e.pos.y);
	handle->window->draw(s);
}


void SfmlView::handle_event(Event* e) {
	std::cerr << " [ V<  M   C ] SfmlView received: " << e->name() << "\n";
	if (state == model::EXIT) return;
	if (is_concurrent()) {
		// no ticks are being sent if we're concurrent
		queue.push(e);
	} else {
		_handle_event(this, *e);
		delete e;
		if (state == model::PLAYING) handle->window->display();
	}
}


void SfmlView::loop() {
	while (true) switch (state) {
		case model::PLAYING:
			game_mode();
			break;
		case model::EXIT:
			return;
		default:
			text_mode();
			break;
	}
}


void SfmlView::game_mode() {
	auto ts = game->entity_lock.new_timestamp();
	while (state == model::PLAYING) {
		if (game->entity_lock.read_lock(ts)) {
			handle->window->clear();
			redraw(); // always 'expect' a tick
				
			// stats
			if (ticks%50001 == 0) {
				std::cout << "SfmlView: Did 50000 ticks\n";
				ticks = 1;
			} else {
				ticks++;
			}
			
			while (not queue.empty()) {
				Event* e = queue.pop();
				_handle_event(this, *e);
				delete e;
			}
			game->entity_lock.read_unlock(ts);
			
			handle->window->display();
		} else {
			std::cout << "SfmlView: read_lock has been closed\n";
			return;
		}
	}
}


void SfmlView::text_mode() {
	while (state != model::PLAYING and state != model::EXIT) {
		while (not queue.empty()) {
			Event* e = queue.pop();
			_handle_event(this, *e);
			delete e;
		}
	}
	std::cout << "SfmlView: exiting text mode\n";
}



