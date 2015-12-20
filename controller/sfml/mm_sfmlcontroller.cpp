
#include "sfmlcontroller.hpp"

#include "game/game.hpp"
#include "view/sfml/sfmlview.hpp"

using namespace si;
using namespace si::view;

namespace si {
namespace controller {

BEGIN_SPECIALIZATION(_handle_event, void, SfmlController* m, const Event& e) {
	std::cout << "SfmlView: Unhandled Event occured\n";
} END_SPECIALIZATION;


// A rather big specialization, because it handles all sfml events
BEGIN_SPECIALIZATION(_handle_event, void, SfmlController* c, const SfmlInput& e) {
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
								c->handle->view->handle_event(new DisplayText("Sorry, there are no more spots available", DisplayState::ERROR));
								std::cout << "No more players available\n";
							} else {
								std::cout << "got player " << c->my_player << "\n";
								c->output_queue.push(new CreatePlayer(c->my_player));
								c->handle->view->handle_event(new SfmlReady);
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
							c->output_queue.push(new SetDirection(c->my_player, util::WEST));
							break;
						case sf::Keyboard::Right:
							c->output_queue.push(new SetDirection(c->my_player, util::EAST));
							break;
						case sf::Keyboard::Space:
							c->output_queue.push(new Fire(c->my_player));
						default:
							break;
					}
					break;
				case sf::Event::KeyReleased:
					switch(se.key.code) {
						case sf::Keyboard::Left:
						case sf::Keyboard::Right:
							c->output_queue.push(new SetDirection(c->my_player, util::HOLD));
							break;
						default:
							break;
					}
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


BEGIN_SPECIALIZATION(_handle_event, void, SfmlController* c, const ModelStateChange& e) {
	// we wait for a player whether model is already going or not
// 	if (c->state != model::PRE_WAIT) {
// 		c->state = e.state;
// 	}
	std::cout << "SfmlController: Got ModelStateChange\n";
} END_SPECIALIZATION;

BEGIN_SPECIALIZATION(_handle_event, void, SfmlController* c, const SfmlExit& e) {
	c->state = model::EXIT;
	c->output_queue.push(new ReleasePlayer(c->my_player));
	c->handle->view->handle_event(e.clone());
	c->done.set();
} END_SPECIALIZATION;


BEGIN_SPECIALIZATION(_handle_event, void, SfmlController* c, const ModelStart& e) {
	std::cout << "SfmlController: got ModelStart\n";
	c->handle->view->handle_event(new ModelStart); // it is possible it will get this message twice, that doesn't matter though
	c->handle->view->handle_event(new DisplayText("Press space to start\n"));
	c->state = model::WAIT;
} END_SPECIALIZATION;


}
}
