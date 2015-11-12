
/* [bake me]

dependencies["headers"] = [
]

dependencies["build_objects"] = [
	"controller/sfml>>headers",
	"view/sfml>>headers",
]

[stop baking] */

#pragma once

#include <cassert>

#include "SFML/Graphics.hpp"

namespace si {

// forward declaration
namespace controller { class SfmlController; }
namespace view { class SfmlView; }

namespace vc {

class SfmlVc;

// Base class for both SfmlView as SfmlController
class SfmlBase {
public:
	friend SfmlVc;
	
	// might or might not block for a long time
	void start();
	
private:
	vc::SfmlVc* handle = nullptr;
};



class SfmlVc {
public:
	SfmlVc(unsigned int width=500, unsigned int height=500):
		window(sf::VideoMode(width, height), "Space Invaders") {}
	
	void couple_controller(si::controller::SfmlController* _controller);
	void decouple_controller();
	
	void couple_view(si::view::SfmlView* _view);
	void decouple_view();
	
	// Will block for a long time (usually)
	void start();
	
	sf::RenderWindow window;
	
private:
	volatile bool running = false;
	
	si::controller::SfmlController* controller = nullptr;
	si::view::SfmlView* view = nullptr;
};


}
}
