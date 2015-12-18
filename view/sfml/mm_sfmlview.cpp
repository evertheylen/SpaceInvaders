
#include "sfmlview.hpp"
#include "game/game.hpp"

using namespace si;
using namespace si::view;

namespace si {
namespace view {


BEGIN_SPECIALIZATION(_handleEvent, void, SfmlView* m, Event& e) {
	std::cout << "SfmlView: Unhandled Event occured\n";
} END_SPECIALIZATION;

BEGIN_SPECIALIZATION(_handleEvent, void, SfmlView* m, Tick& e) {
	if (m->state == model::PLAYING) {
		m->handle->window.clear();
		m->redraw(); // always 'expect' a tick
	}
} END_SPECIALIZATION;


BEGIN_SPECIALIZATION(_handleEvent, void, SfmlView* m, ModelStart& e) {
	std::cout << "SfmlView Init\n";
	m->handle->window.setActive(true);
	m->handle->window.clear();
	m->handle->window.display();
} END_SPECIALIZATION;


BEGIN_SPECIALIZATION(_handleEvent, void, SfmlView* m, DisplayText& e) {
	std::cout << "SfmlView: display text " << e.message << "\n";
	sf::Text text;
	text.setFont(m->res.font);
	text.setString(e.message);
	text.setCharacterSize(24); // in pixels, not points!
	if (e.state == DisplayState::ERROR) {
		text.setColor(sf::Color::Red);
	} else {
		text.setColor(sf::Color::Green);
	}
	m->handle->window.draw(text);
	m->handle->window.display();
} END_SPECIALIZATION;


BEGIN_SPECIALIZATION(_handleEvent, void, SfmlView* m, SfmlExit& e) {
	if (m->handle->window.isOpen()) m->handle->window.close();
	m->state = model::EXIT;
} END_SPECIALIZATION;


BEGIN_SPECIALIZATION(_handleEvent, void, SfmlView* m, SfmlReady& e) {
	std::cout << "SfmlView starts playing\n";
	m->state = model::PLAYING;
} END_SPECIALIZATION;


BEGIN_SPECIALIZATION(_handleEvent, void, SfmlView* m, LevelStart& e) {
	if (m->state == model::WAIT) {
		std::cout << "We wait for the controller\n";
	} else {
		std::cout << "SfmlView starts playing\n";
		m->state = m->game->get_model().get_state();
	}
} END_SPECIALIZATION;


BEGIN_SPECIALIZATION(_handleEvent, void, SfmlView* m, LevelEnd& e) {
	std::cout << "SfmlView stops playing\n";
	m->state = model::RECAP;
} END_SPECIALIZATION;


}
}
