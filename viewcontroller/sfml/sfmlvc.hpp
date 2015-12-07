
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
#include <thread>

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
	std::vector<std::thread*> start();
	
protected:
	vc::SfmlVc* handle = nullptr;
};



class SfmlVc {
public:
	// SfmlVc should already be in a usable state after the constructor (obviously)
	SfmlVc(unsigned int width=800, unsigned int height=600);
	
	SfmlVc(const SfmlVc& other) = delete;
	
	void couple_controller(si::controller::SfmlController* _controller);
	void decouple_controller();
	
	void couple_view(si::view::SfmlView* _view);
	void decouple_view();
	
	std::vector<std::thread*> start();
	
	sf::RenderWindow window;
	
private:
	// Will block for a long time (usually)
	void loop();
	
	// multithreaded stuff
	std::atomic<std::thread*> running_thread{nullptr};
	
	si::controller::SfmlController* controller = nullptr;
	si::view::SfmlView* view = nullptr;
};


}
}
