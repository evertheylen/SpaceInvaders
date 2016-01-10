
#include "game.hpp"

#include <cassert>
#include <thread>
#include <set>
#include <memory>
#include <utility>


using namespace si;

Game::Game(const picojson::value& conf) {
	the_model = model::Model(conf, this);
}

void Game::notify_controllers(Event* e) {
	std::cerr << " [ V   M-->C ] Passing to all Controllers: " << e->name() << "\n";
	for (controller::Controller* c: controllers) {
		c->handle_event(e);
	}
}

void Game::notify_all(Event* e) {
	notify_views(e->clone());
	notify_controllers(e->clone());
	delete e;
}


void Game::notify_views(Event* e) {
	std::cerr << " [ V<--M   C ] Passing to all Views: " << e->name() << "\n";
	for (view::View* v: views) {
		if (v->is_concurrent()) {
			assert(dynamic_cast<Tick*>(e) == nullptr);
			v->handle_event(e->clone());
		} else {
			v->handle_event(e->clone());
		}
	}
	delete e;
}

void Game::notify_views(Tick* e) {
	for (view::View* v: synchr_views) {
		std::cerr << " [ V<--M   C ] Passing Tick to some View\n";
		v->handle_event(e->clone());
	}
	delete e;
}

Event* Game::get_input_event() {
	for (controller::Controller* c: controllers) {
		if (Event* e = c->get_event()) {
			return e;
		}
	}
	if (not model_input.empty()) {
		return model_input.pop();
	}
	return nullptr;
}


void Game::register_view(view::View* v) {
	views.insert(v);
	if (not v->is_concurrent()) {
		synchr_views.insert(v);
	}
}

void Game::unregister_view(view::View* v) {
	views.erase(v);
}

void Game::register_controller(controller::Controller* c) {
	controllers.insert(c);
}

void Game::unregister_controller(controller::Controller* c) {
	controllers.erase(c);
}

const model::Model& Game::get_model() const {
	return the_model;
}


void Game::run() {
	std::set<std::thread*> threads;
	
	// starting happens synchronously!
	for (view::View* v: views) threads += v->start();
	for (controller::Controller* c: controllers) threads += c->start();
	
	// Now that all views / controllers are running, give them an extra Init event
	// to initialize whatever they want...
	notify_views(new Init);
	notify_controllers(new Init);
	
	std::set<std::thread*> model_threads;
	model_threads += the_model.start();
	// ... work is being done ...
	// ... lots of Aliens are being murdered ...
	
	for (view::View* v: views) v->wait_until_done();
	for (controller::Controller* c: controllers) c->wait_until_done();
	
	for (std::thread* t: threads) {
		// join on all threads
		if (t != nullptr) {
			t->join();
			delete t;
		}
	}
	
	std::cout << "Waiting for model to stop...\n";
	
	// all views / controllers are done
	// signal to the model it should quit
	model_input.push(new si::VCStop);
	for (std::thread* t: model_threads) {
		if (t != nullptr) {
			t->join();
			delete t;
		}
	}
	
}


void Game::model_lock() {
	model_mutex.lock();
}

void Game::model_unlock() {
	model_mutex.unlock();
}


