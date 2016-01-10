
#include "sfmlcontroller.hpp"

#include "game/game.hpp"
#include "view/sfml/sfmlview.hpp"

using namespace si;
using namespace si::view;

namespace si {
namespace controller {

BEGIN_SPECIALIZATION(_handle_event, void, SfmlController* m, const Event& e) {
	std::cout << "SfmlController: Unhandled Event occured\n";
} END_SPECIALIZATION;


// A rather big specialization, because it handles all sfml events
BEGIN_SPECIALIZATION(_handle_event, void, SfmlController* c, const SfmlInput& e) {
	sf::Event se = e.event;
	switch (c->phase) {
		case model::PRE_WAIT: {
			switch (se.type) {
				// key pressed
				case sf::Event::KeyPressed:
					switch(se.key.code) {
						case sf::Keyboard::Space:
							std::cout << "SfmlController (" << c << "): Asking for player\n";
							c->output_queue.push(new AskPlayer(c));
							c->phase = c->model_state.phase();
							break;
						default:
							break;
					}
					break;
				default:
					break;
			}
		}; break;
		// no sfml input events are handled when WAIT-ing (we're waiting for a CreatePlayer event)
		case model::PLAYING: {
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
		case model::RECAP:
		case model::GAMEOVER: {
			switch (se.type) {
				// key pressed
				case sf::Event::KeyPressed:
					switch(se.key.code) {
						case sf::Keyboard::Space:
							std::cout << "SfmlController (" << c << "): stop recap/gameover\n";
							c->output_queue.push(new Ready);
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
	if (c->update_state(e.state)) {
		if (c->my_player < 0) {
			std::cout << "SfmlController (" << c << "): Model changed state, (" << c->model_state.phase()
					  << ") however, we don't have a player yet.\n";
			return; // avoiding the phase set
		}
		switch (c->model_state.phase()) {
			case model::WAIT:
				// model has started!
				std::cout << "SfmlController (" << c << "): Model has started it seems\n";
				break;
			case model::PLAYING:
				std::cout << "SfmlController (" << c << "): Model is playing it seems\n";
				break;
			case model::RECAP:
				std::cout << "SfmlController (" << c << "): Model is recapping it seems\n";
				break;
			default:
				break;
		}
		
		// as long as we're not pre-waiting, we can update our own phase
		c->phase = c->model_state.phase();
	}
} END_SPECIALIZATION;


BEGIN_SPECIALIZATION(_handle_event, void, SfmlController* c, const CreatePlayer& e) {
	std::cout << "SfmlController (" << c << "): Got player " << e.ID << "\n";
	c->my_player = e.ID;
	// update state
	ModelStateChange msc(c->model_state);
	_handle_event(c, msc);
	std::cout << "SfmlController (" << c << "): After CreatePlayer event, we're in state " << c->phase << "\n";
} END_SPECIALIZATION;


BEGIN_SPECIALIZATION(_handle_event, void, SfmlController* c, const SfmlExit& e) {
	c->phase = model::EXIT;
	c->output_queue.push(new ReleasePlayer(c->my_player));
	c->handle->view->handle_event(e.clone());
	c->done.set();
} END_SPECIALIZATION;



}
}
