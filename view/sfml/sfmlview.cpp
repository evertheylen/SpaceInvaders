
#include "sfmlview.hpp"
#include "game/game.hpp"

using namespace si;
using namespace si::view;

SfmlView::SfmlView(Game* g): game(g) {
	assert(SfmlView::res != nullptr);
	backgroundsprite.setTextureRect(sf::IntRect(0,0,800,600));
	backgroundsprite.setTexture(res->background);
}

std::thread* SfmlView::start() {
	// this is where we draw
	std::cout << "View has started\n";
	// TODO draw from this thread lol
	handle->window.setActive(false);
	//redraw();
	
	return SfmlBase::start();
}

void SfmlView::redraw() {
	handle->window.clear();
	
	handle->window.draw(backgroundsprite);
	
	// TODO remove this, draw from the right thread
	handle->window.setActive(true);
	//std::cout << "Trying to draw\n";
	
	// initialize all sprites
	const auto& model = game->get_model();
	for (const auto& uniq_e: model.entities) {
		si::model::Entity* e = uniq_e.get();
		sprites[e] = sf::Sprite();
		//sprites[e].setTextureRect(sf::IntRect(e->x, e->y, 64, 64));
		sprites[e].setTexture(SfmlView::res->player);
		sprites[e].setPosition(e->pos.x, e->pos.y);
		handle->window.draw(sprites[e]);
	}
	
	handle->window.display();
}


void SfmlView::handleEvent(Event* e) {
	if (Redraw* r = dynamic_cast<Redraw*>(e)) {
		redraw();
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

