
#include "sfmlcontroller.hpp"

#include "game/game.hpp"
#include "view/sfml/sfmlview.hpp"

using namespace si;
using namespace si::view;

namespace si {
namespace controller {

// A rather big specialization, because it handles all sfml events
BEGIN_SPECIALIZATION(_handleEvent, void, SfmlController* c, SfmlInput& e) {
	sf::Event se = e.event;
	switch (c->state) {
		case model::State::WAIT: {
			switch (se.type) {
				// key pressed
				case sf::Event::KeyPressed:
					switch(se.key.code) {
						case sf::Keyboard::Space:
							c->my_player = c->game->get_player();
							if (c->my_player < 0) {
								c->handle->get_view()->handleEvent(new DisplayText("Sorry, there are no more spots available", DisplayState::ERROR));
								std::cout << "No more players available\n";
							} else {
								std::cout << "got player " << c->my_player << "\n";
								c->game->notifyModel(new CreatePlayer(c->my_player));
								c->handle->get_view()->handleEvent(new SfmlReady);
								c->state = model::PLAYING;
								return;
							}
							break;
						default:
							break;
					}
					break;
				default:
					break;
			}
		}; break;
		case model::State::PLAYING: {
			switch (se.type) {
				// key pressed
				case sf::Event::KeyPressed:
					switch(se.key.code) {
						case sf::Keyboard::Left:
							c->game->notifyModel(new SetDirection(c->my_player, util::WEST));
							break;
						case sf::Keyboard::Right:
							c->game->notifyModel(new SetDirection(c->my_player, util::EAST));
							break;
						default:
							break;
					}
					break;
				case sf::Event::KeyReleased:
					c->game->notifyModel(new SetDirection(c->my_player, util::HOLD));
				// we don't process other types of events
				default:
					break;
			}
		}; break;
		case model::State::RECAP: {
			switch (se.type) {
				// key pressed
				case sf::Event::KeyPressed:
					switch(se.key.code) {
						case sf::Keyboard::Space:
							std::cout << "SfmlController: stop recap\n";
							break;
						default:
							break;
					}
					break;
				default:
					break;
			}
		}; break;
		default:
			break;
	}
} END_SPECIALIZATION;


BEGIN_SPECIALIZATION(_handleEvent, void, SfmlController* c, ModelStateChange& e) {
	// we wait for a player whether model is already going or not
// 	if (c->state != model::WAIT) {
// 		c->state = e.state;
// 	}
	std::cout << "SfmlController: Got ModelStateChange\n";
} END_SPECIALIZATION;

BEGIN_SPECIALIZATION(_handleEvent, void, SfmlController* c, SfmlExit& e) {
	c->state = model::EXIT;
	c->game->notifyModel(new ReleasePlayer(c->my_player));
	c->handle->get_view()->handleEvent(e.clone());
} END_SPECIALIZATION;


BEGIN_SPECIALIZATION(_handleEvent, void, SfmlController* c, ModelStart& e) {
	std::cout << "SfmlController: got ModelStart\n";
	c->handle->get_view()->handleEvent(new DisplayText("Press space to start\n"));
	c->state = model::WAIT;
} END_SPECIALIZATION;

BEGIN_SPECIALIZATION(_handleEvent, void, SfmlController* m, Event& e) {
	std::cout << "SfmlView: Unhandled Event occured\n";
} END_SPECIALIZATION;


}
}
