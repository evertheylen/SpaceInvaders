
#include "sfmlview.hpp"
#include "game/game.hpp"
#include "util/util.hpp"

using namespace si;
using namespace si::view;

SfmlView::SfmlView(Game* g, bool _concurrent):  View(_concurrent), game(g) {
	assert(SfmlView::res != nullptr);
	backgroundsprite.setTextureRect(sf::IntRect(0,0,800,600));
	backgroundsprite.setTexture(res->background);
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
	//sf::Context context;
	if (game->entity_lock.read_lock()) {
		if (handle->window.isOpen()) {
			handle->window.clear();
			
			handle->window.draw(backgroundsprite);
			
			// TODO remove this, draw from the right thread
			//std::cout << "Trying to draw\n";
			
			// initialize all sprites
			for (const auto& uniq_e: game->get_model().all_entities()) {
				si::model::Entity* e = uniq_e.get();
				sprites[e] = sf::Sprite();
				//sprites[e].setTextureRect(sf::IntRect(e->x, e->y, 64, 64));
				if (dynamic_cast<si::model::Player*>(e)) {
					sprites[e].setTexture(SfmlView::res->player);
				} else {
					sprites[e].setTexture(SfmlView::res->big_alien);
				}
				sprites[e].setPosition(e->pos.x, e->pos.y);
				handle->window.draw(sprites[e]);
			}
			
			handle->window.display();
		}
		game->entity_lock.read_unlock();
	}
}

/*
// public
void SfmlView::wake_up() {
	if (isConcurrent()) {
		std::unique_lock<std::mutex> locker(sleep_lock);
		sleep_cv.notify_one();
	}
}


// private
void SfmlView::sleep() {
	if (isConcurrent()) {
		std::unique_lock<std::mutex> locker(sleep_lock);
		sleep_cv.wait(locker);
	}
}
*/

void SfmlView::handleEvent(Event* e) {
	if (isConcurrent()) {
		// no ticks are being sent if we're concurrent
		queue.push(e);
		//wake_up();
	} else {
		if (dynamic_cast<Tick*>(e)) {
			redraw();
		} else {
			// dispatch the event!
			_handleEvent(this, *e);
		}
		delete e;
	}
}


void SfmlView::loop() {
	while (true) {
		//std::cout << "SfmlView Awake!\n";
		redraw(); // always 'expect' a tick
		
		if (not queue.empty()) {
			Event* e = queue.pop();
			_handleEvent(this, *e);
			delete e;
		}
		//std::this_thread::sleep_for(std::chrono::milliseconds(1));
		//sleep();  // wake_up wakes this thread back up obv
		
		// TODO break out of this elegantly
	}
}


// ---- STATICS ----

// definition...
SfmlResources* SfmlView::res = nullptr;

void SfmlView::load_resources() {
	res = new SfmlResources;
}

void SfmlView::unload_resources() {
	delete res;
	res = nullptr;
}

