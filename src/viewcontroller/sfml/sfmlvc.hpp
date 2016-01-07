
#pragma once

#include <cassert>
#include <atomic>
#include <mutex>
#include <thread>

#include "SFML/Graphics.hpp"

#include "viewcontroller/viewcontroller.hpp"
#include "event/event.hpp"
#include "util/flag/flag.hpp"

namespace si {

// forward declaration
namespace controller { class SfmlController; }
namespace view { class SfmlView; }

namespace vc {

class SfmlVc: public ViewController {
public:
	SfmlVc() = default;
	
	SfmlVc(const SfmlVc& other) = delete;
	
	void init(int width=800, int height=600);
	
	void couple_controller(si::controller::SfmlController* _controller);
	void decouple_controller();
	
	void couple_view(si::view::SfmlView* _view);
	void decouple_view();
	
	std::vector<std::thread*> start();
	
	~SfmlVc();
	
//protected:  // friending the other classes simply does not work it seems
	
	// no `friend class` here because of forward declaration
	friend view::SfmlView;
	friend controller::SfmlController;
	
	// Will block for a long time (usually)
	// Only called in a multithreaded SFML:VC
	void loop(util::Flag& f);
	
	sf::RenderWindow* window = nullptr;
	
	// multithreaded stuff
	std::atomic<std::thread*> running_thread{nullptr};
	
	si::controller::SfmlController* controller = nullptr;
	si::view::SfmlView* view = nullptr;
};


}
}
