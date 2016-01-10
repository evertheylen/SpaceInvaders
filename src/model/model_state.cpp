
#include "model_state.hpp"

using namespace si::model;

State::State() {
	_phase = PRE_WAIT;
}

State::~State() {
	clear();
}

Phase State::phase() const {
	return _phase;
}

void State::set(Phase new_phase) {
	clear();
	_phase = new_phase;
	init();
}

State::State(const State& other) {
	_phase = other._phase;
	init();
	copy(other);
}

State& State::operator=(const State& other) {
	clear();
	_phase = other._phase;
	init();
	copy(other);
	return *this;
}

bool State::operator==(const State& other) const {
	if (_phase != other._phase)
		return false;
	switch (_phase) {
		case PLAYING:
			return playing == other.playing;
		case RECAP:
			return recap == other.recap;
		case GAMEOVER:
			return gameover == other.gameover;
		default:
			return true;
	}
}

bool State::operator!=(const State& other) const {
	return not operator==(other);
}


void State::clear() {
	switch (_phase) {
		case PLAYING:
			playing.~PlayingInfo();
			break;
		case RECAP:
			recap.~RecapInfo();
			break;
		case GAMEOVER:
			gameover.~GameOverInfo();
			break;
		default:
			break;
	}
}

void State::copy(const State& other) {
	_phase = other._phase;
	switch (_phase) {
		case PLAYING:
			playing = other.playing;
			break;
		case RECAP:
			recap = other.recap;
			break;
		case GAMEOVER:
			gameover = other.gameover;
			break;
		default:
			break;
	}
}

void State::init() {
	// oh well, 1337 H4CK3R5
	switch (_phase) {
		case PLAYING:
			new (&playing) PlayingInfo();
			break;
		case RECAP:
			new (&recap) RecapInfo();
			break;
		case GAMEOVER:
			new (&gameover) GameOverInfo();
			break;
		default:
			break;
	}
}
