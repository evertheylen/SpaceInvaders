
#pragma once

#include <set>
#include <map>
#include <iostream>


namespace si {
namespace model {

// some of these phases are not used by model (i.e. PRE_WAIT)
enum Phase { PRE_WAIT, WAIT, PLAYING, RECAP, GAMEOVER, EXIT };

class State {
public:
	State() {
		_phase = WAIT;
	}
	
	~State() {
		clear();
	}
	
	Phase phase() {
		return _phase;
	}
	
	void set(Phase new_phase) {
		clear();
		_phase = new_phase;
		init();
	}
	
	State(State& other) {
		_phase = other._phase;
		init();
		copy(other);
	}
	
	State& operator=(State& other) {
		clear();
		_phase = other._phase;
		init();
		copy(other);
		return *this;
	}
	
	bool operator==(State& other) {
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
	
	
	struct PlayingInfo {
		std::set<int> player;
		bool operator==(PlayingInfo& o) { return player == o.player; }
	};
	
	struct RecapInfo {
		std::map<int, int> scores;
		bool operator==(RecapInfo& o) { return scores == o.scores; }
	};
	
	struct GameOverInfo {
		bool victory;
		bool operator==(GameOverInfo& o) { return victory == o.victory; }
	};
	
	union {
		PlayingInfo playing;
		RecapInfo recap;
		GameOverInfo gameover;
	};
	
private:
	void clear() {
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
	
	void copy(State& other) {
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
	
	void init() {
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
	
	Phase _phase;
};

}
}
