
#include "sfmlview.hpp"
#include "game/game.hpp"

using namespace si;
using namespace si::view;
using namespace si::model;

namespace si {
namespace view {


BEGIN_SPECIALIZATION(_handle_event, void, SfmlView* v, const Event& e) {
	std::cout << "SfmlView: Unhandled Event occured\n";
} END_SPECIALIZATION;

BEGIN_SPECIALIZATION(_handle_event, void, SfmlView* v, const Tick& e) {
	if (v->phase == model::PLAYING) {
		v->handle->window->clear();
		v->redraw(); // always 'expect' a tick
	}
	
	if (not v->concurrent and v->handle->controller == nullptr) {
		// if there is no controller, and we're not concurrent, that
		// means we have to process some input ourselves
		sf::Event e;
		while (v->handle->window->isOpen() and v->handle->window->pollEvent(e)) {
			if (e.type == sf::Event::Closed) {
				std::cout << "SfmlView: got close sf::Event\n";
				v->handle_event(new si::SfmlExit);
			}
		}
	}
} END_SPECIALIZATION;


BEGIN_SPECIALIZATION(_handle_event, void, SfmlView* v, const DisplayText& e) {
	std::cout << "SfmlView: display text " << e.message << "\n";
	sf::Text text;
	text.setFont(v->res.font);
	text.setString(e.message);
	text.setCharacterSize(24); // in pixels, not points!
	if (e.state == DisplayState::ERROR) {
		text.setColor(sf::Color::Red);
	} else {
		text.setColor(sf::Color::Green);
	}
	v->handle->window->draw(text);
	v->handle->window->display();
} END_SPECIALIZATION;


BEGIN_SPECIALIZATION(_handle_event, void, SfmlView* v, const SfmlExit& e) {
	if (v->handle->window->isOpen()) v->handle->window->close();
	v->phase = model::EXIT;
	v->done.set();
} END_SPECIALIZATION;



BEGIN_SPECIALIZATION(_handle_event, void, SfmlView* v, const ModelStateChange& e) {
	if (v->update_state(e.state)) {
		switch (v->model_state.phase()) {
			case model::WAIT:
				// model has started!
				std::cout << "SfmlView: Model has started it seems\n";
				break;
			case model::PLAYING:
				std::cout << "SfmlView: Model is playing it seems\n";
				break;
			case model::RECAP:
				std::cout << "SfmlView: Model is recapping it seems\n";
				break;
			default:
				break;
		}
		
		v->phase_change(v->model_state.phase());
	}
} END_SPECIALIZATION;


// Drawing

BEGIN_SPECIALIZATION(_draw, void, SfmlView* v, const Entity& e) {
	std::cout << "SfmlView: trying to draw an entity without specialization\n";
} END_SPECIALIZATION;


BEGIN_SPECIALIZATION(_draw, void, SfmlView* v, const Player& e) {
	v->simple_draw(v->res.player, e);
} END_SPECIALIZATION;


BEGIN_SPECIALIZATION(_draw, void, SfmlView* v, const Alien& e) {
	v->simple_draw(v->res.big_alien, e);
} END_SPECIALIZATION;


BEGIN_SPECIALIZATION(_draw, void, SfmlView* v, const Bullet& e) {
	v->simple_draw(v->res.bullet, e);
} END_SPECIALIZATION;


BEGIN_SPECIALIZATION(_draw, void, SfmlView* v, const Bomb& e) {
	v->simple_draw(v->res.big_bomb, e);
} END_SPECIALIZATION;


}
}
