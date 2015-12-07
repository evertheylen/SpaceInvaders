
#include "sfmlview.hpp"
#include "game/game.hpp"
#include "util/util.hpp"

using namespace si;
using namespace si::view;

SfmlView::SfmlView(Game* g): game(g) {
	assert(SfmlView::res != nullptr);
	backgroundsprite.setTextureRect(sf::IntRect(0,0,800,600));
	backgroundsprite.setTexture(res->background);
}

std::vector<std::thread*> SfmlView::start() {
	std::cout << "View has started\n";
	return SfmlBase::start() + std::vector<std::thread*>{new std::thread(&SfmlView::loop, this)};
}

void SfmlView::redraw() {
	//sf::Context context;
	if (handle->window.isOpen()) {
		handle->window.clear();
		
		handle->window.draw(backgroundsprite);
		
		// TODO remove this, draw from the right thread
		handle->window.setActive(true);
		//std::cout << "Trying to draw\n";
		
		// initialize all sprites
		const auto& model = game->get_model();
		for (const auto& uniq_e: model.all_entities()) {
			si::model::Entity* e = uniq_e.get();
			sprites[e] = sf::Sprite();
			//sprites[e].setTextureRect(sf::IntRect(e->x, e->y, 64, 64));
			sprites[e].setTexture(SfmlView::res->player);
			sprites[e].setPosition(e->pos.x, e->pos.y);
			handle->window.draw(sprites[e]);
		}
		
		handle->window.display();
	}
}

// public
void SfmlView::wake_up() {
	std::unique_lock<std::mutex> locker(sleep_lock);
	sleep_notified = true;
	sleep_cv.notify_one();
}


// private
void SfmlView::sleep() {
	std::unique_lock<std::mutex> locker(sleep_lock);
	while(not sleep_notified) {  // used to avoid spurious wakeups
		sleep_cv.wait(locker);
	}
	sleep_notified = false;
}


void SfmlView::handleEvent(Event* e) {
	if (Redraw* r = dynamic_cast<Redraw*>(e)) {
		redraw();
	}
}

void SfmlView::loop() {
	handle->window.setActive(true);
	while (true) {
		std::cout << "SfmlView Awake!\n";
		Event* e = game->get_view_event(this);
		if (e != nullptr) {
			redraw();
		}
		delete e;
		//std::this_thread::sleep_for(std::chrono::milliseconds(1));
		sleep();  // wake_up wakes this thread back up obv
		
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

