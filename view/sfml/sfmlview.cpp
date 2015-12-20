
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
	
	// initialize all sprites
	for (const auto& e: game->get_model().all_entities()) {
		sprites[e] = sf::Sprite();
		//sprites[e].setTextureRect(sf::IntRect(e->x, e->y, 64, 64));
		// TODO replace with yomm
		if (dynamic_cast<si::model::Player*>(e)) {
			sprites[e].setTexture(res.player);
		} else {
			sprites[e].setTexture(res.big_alien);
		}
		sprites[e].setPosition(e->pos.x, e->pos.y);
		handle->window->draw(sprites[e]);
	}
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
	auto ts = game->entity_lock.new_timestamp();
	while (true) {
		if (game->entity_lock.read_lock(ts)) {
			if (state == model::PLAYING) {
				handle->window->clear();
				redraw(); // always 'expect' a tick
				
				// stats
				if (ticks%50001 == 0) {
					std::cout << "SfmlView: Did 50000 ticks\n";
					ticks = 1;
				} else {
					ticks++;
				}
			}
			
			if (not queue.empty()) {
				Event* e = queue.pop();
				_handle_event(this, *e);
				delete e;
			}
			game->entity_lock.read_unlock(ts);
			if (state == model::EXIT) {
				return; // thereby closing this sfmlview
			}
			if (state == model::PLAYING) handle->window->display();
		}
	}
}


