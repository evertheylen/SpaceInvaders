
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
	if (v->state == model::PLAYING) {
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
	v->state = model::EXIT;
	v->done.set();
} END_SPECIALIZATION;



BEGIN_SPECIALIZATION(_handle_event, void, SfmlView* v, const ModelStart& e) {
	std::cout << "SfmlView: got ModelStart\n";
	if (v->state != model::PRE_WAIT) {
		if (not v->concurrent and v->handle->controller == nullptr)
			v->handle->init();
		v->handle->window->setActive(true);
		v->handle->window->clear(sf::Color::Blue);
		v->handle->window->display();
		v->state = model::WAIT;
		std::cout << "SfmlView: ModelStart handled\n";
	}
} END_SPECIALIZATION;


BEGIN_SPECIALIZATION(_handle_event, void, SfmlView* v, const GameStop& e) {
	v->state = model::GAMEOVER;
	std::cout << "SfmlView: Game Over received, value = " << e.victory << "\n";
} END_SPECIALIZATION;


BEGIN_SPECIALIZATION(_handle_event, void, SfmlView* v, const Recap& e) {
	std::cout << "SfmlView stops playing\n";
	v->state = model::RECAP;
} END_SPECIALIZATION;


BEGIN_SPECIALIZATION(_handle_event, void, SfmlView* v, const LevelStart& e) {
	std::cout << "SfmlView starts playing\n";
	v->state = model::PLAYING;
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



}
}
