
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
							std::cout << "SfmlController (" << c << "): Getting player\n";
							c->my_player = c->game->get_player();
							if (c->my_player < 0) {
								c->handle->view->handle_event(new DisplayText("Sorry, there are no more spots available", DisplayState::ERROR));
								std::cout << "SfmlController (" << c << "): No more players available\n";
							} else {
								std::cout << "SfmlController (" << c << "): got player " << c->my_player << "\n";
								c->output_queue.push(new CreatePlayer(c->my_player)); // if model is waiting, this is also interpreted as Ready
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
		case model::State::RECAP:
		case model::State::GAMEOVER: {
			switch (se.type) {
				// key pressed
				case sf::Event::KeyPressed:
					switch(se.key.code) {
						case sf::Keyboard::Space:
							std::cout << "SfmlController (" << c << "): stop recap/gameover\n";
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



BEGIN_SPECIALIZATION(_handle_event, void, SfmlController* c, const SfmlExit& e) {
	c->state = model::EXIT;
	c->output_queue.push(new ReleasePlayer(c->my_player));
	c->handle->view->handle_event(e.clone());
	c->done.set();
} END_SPECIALIZATION;


BEGIN_SPECIALIZATION(_handle_event, void, SfmlController* c, const ModelStart& e) {
	std::cout << "SfmlController (" << c << "): got ModelStart\n";
	c->handle->view->handle_event(new ModelStart); // it is possible it will get this message twice, that doesn't matter though
	c->handle->view->handle_event(new DisplayText("Press space to start\n"));
	c->state = model::WAIT;
} END_SPECIALIZATION;


BEGIN_SPECIALIZATION(_handle_event, void, SfmlController* c, const LevelStart& e) {
	if (c->my_player >= 0) { // only if actually playing
		c->state = model::PLAYING;
	} else {
		std::cout << "SfmlController (" << c << "): not starting, we don't have a player yet\n";
	}
} END_SPECIALIZATION;


BEGIN_SPECIALIZATION(_handle_event, void, SfmlController* c, const GameStop& e) {
	c->state = model::GAMEOVER;
	std::cout << "SfmlController (" << c << "): Game Over received, value = " << e.victory << "\n";
} END_SPECIALIZATION;


BEGIN_SPECIALIZATION(_handle_event, void, SfmlController* c, const Recap& e) {
	c->state = model::RECAP;
} END_SPECIALIZATION;


}
}
