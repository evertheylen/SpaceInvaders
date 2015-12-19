
#include "sfmlview.hpp"
#include "game/game.hpp"
#include "util/util.hpp"

using namespace si;
using namespace si::view;

SfmlView::SfmlView(Game* g, bool _concurrent):  View(_concurrent), game(g) {
	backgroundsprite.setTextureRect(sf::IntRect(0,0,800,600));
	backgroundsprite.setTexture(res.background);
}

std::vector<std::thread*> SfmlView::start() {
	std::cout << "View has started\n";
	handle->window.setActive(false);
	if (isConcurrent()) {
		return SfmlBase::start() + std::vector<std::thread*>{new std::thread(&SfmlView::loop, this)};
	} else {
		return SfmlBase::start();
	}
}

void SfmlView::redraw() {
	handle->window.draw(backgroundsprite);
	
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
		handle->window.draw(sprites[e]);
	}
}

void SfmlView::handleEvent(Event* e) {
	if (isConcurrent()) {
		// no ticks are being sent if we're concurrent
		queue.push(e);
		//wake_up();
	} else {
		handle->window.setActive(true);  // TODO make this more elegant
		_handleEvent(this, *e);
		if (state == model::PLAYING) handle->window.display();
		delete e;
		if (state != model::EXIT) handle->window.setActive(false);
	}
}


void SfmlView::loop() {
	auto ts = game->entity_lock.new_timestamp();
	while (true) {
		if (game->entity_lock.read_lock(ts)) {
			if (state == model::PLAYING) {
				handle->window.clear();
				redraw(); // always 'expect' a tick
				
				// stats
				if (ticks%501 == 0) {
					std::cout << "SfmlView: Did 500 ticks ----------\n";
					ticks = 1;
				} else {
					ticks++;
				}
			}
			
			if (not queue.empty()) {
				Event* e = queue.pop();
				_handleEvent(this, *e);
				delete e;
			}
			if (state == model::EXIT) return;
			if (state == model::PLAYING) handle->window.display();
			game->entity_lock.read_unlock(ts);
		}
	}
}


