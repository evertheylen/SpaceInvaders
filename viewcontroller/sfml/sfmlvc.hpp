
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
#include <atomic>
#include <mutex>

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
	
	
protected:
	vc::SfmlVc* handle = nullptr;
};



class SfmlVc {
public:
	// SfmlVc should already be in a usable state after the constructor (obviously)
	SfmlVc(unsigned int width=800, unsigned int height=600);
	
	SfmlVc(const SfmlVc& other) {};
	
	void couple_controller(si::controller::SfmlController* _controller);
	void decouple_controller();
	
	void couple_view(si::view::SfmlView* _view);
	void decouple_view();
	
	// Will block for a long time (usually)
	void start();
	
	sf::RenderWindow window;
	
private:
	// multithreaded stuff
	std::atomic<bool> running{false};
	
	si::controller::SfmlController* controller = nullptr;
	si::view::SfmlView* view = nullptr;
};


}
}
