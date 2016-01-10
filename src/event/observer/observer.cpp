
#include "observer.hpp"

using namespace si;

bool Observer::update_state(const model::State& newstate) {
	if (model_state != newstate) {
		model_state = newstate;
		return true;
	}
	return false;
}

