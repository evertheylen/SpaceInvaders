
#include "sfmlview.hpp"
#include "game/game.hpp"

using namespace si;
using namespace si::view;

SfmlView::SfmlView(Game* g): game(g) {
	assert(SfmlView::res != nullptr);
	backgroundsprite.setTextureRect(sf::IntRect(0,0,800,600));
	backgroundsprite.setTexture(res->background);
}

void SfmlView::start() {
	// this is where we draw
	handle->window.setActive(true);
	
	redraw();
	
	SfmlBase::start();
}

void SfmlView::redraw() {
	handle->window.clear();
	
	handle->window.draw(backgroundsprite);
	
	// initialize all sprites
	game->model_mutex.lock();
	const auto& model = game->get_model();
	for (const auto& uniq_e: model.entities) {
		si::model::Entity* e = uniq_e.get();
		sprites[e] = sf::Sprite();
		//sprites[e].setTextureRect(sf::IntRect(e->x, e->y, 64, 64));
		sprites[e].setTexture(SfmlView::res->player);
		sprites[e].setPosition(e->x, e->y);
		handle->window.draw(sprites[e]);
	}
	game->model_mutex.unlock();
	
	handle->window.display();
}


void SfmlView::handleEvent(Event* e) {
	switch (e->type()) {
		case EventType::REDRAW: {
			redraw();
		} default: {
			break;
		}
	}
}

// definition...
SfmlResources* SfmlView::res = nullptr;

void SfmlView::load_resources() {
	res = new SfmlResources;
}

void SfmlView::unload_resources() {
	delete res;
	res = nullptr;
}

