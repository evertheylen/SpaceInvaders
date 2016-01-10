
#pragma once

#include <set>
#include <map>
#include <iostream>


namespace si {
namespace model {

class Level;

// some of these phases are not used by model (i.e. PRE_WAIT)
enum Phase { PRE_WAIT, WAIT, PLAYING, RECAP, GAMEOVER, EXIT };

class State {
public:
	State();
	
	~State();
	
	Phase phase() const;
	
	void set(Phase new_phase);
	
	State(const State& other);
	
	State& operator=(const State& other);
	
	bool operator==(const State& other) const;
	
	bool operator!=(const State& other) const;
	
	struct PlayingInfo {
		Level* level;
		int levelnum;
		bool operator==(const PlayingInfo& o) const { return level == o.level && levelnum == o.levelnum; }
	};
	
	struct RecapInfo {
		std::map<int, int> scores;
		int levelnum;
		Level* level;
		bool operator==(const RecapInfo& o) const { return scores == o.scores && levelnum == o.levelnum && level == o.level; }
	};
	
	struct GameOverInfo {
		bool victory;
		bool operator==(const GameOverInfo& o) const { return victory == o.victory; }
	};
	
	union {
		PlayingInfo playing;
		RecapInfo recap;
		GameOverInfo gameover;
	};
	
private:
	void clear();
	
	void copy(const State& other);
	
	void init();
	
	Phase _phase;
};

}
}
