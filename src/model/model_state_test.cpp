 
#ifdef TEST_MODEL_STATE
 
#include "model_state.hpp"
#include <iostream>

using namespace std;
using namespace si::model;

int main() {
	State s;
	cout << s.phase() << "\n";
	s.set(PLAYING);
	s.playing.player.insert(5);
	s.playing.player.insert(6);
	for (auto i: s.playing.player) {
		cout << "s.playing: " << i << "\n";
	}
	
	State ss = s;
	for (auto i: ss.playing.player) {
		cout << "ss.playing: " << i << "\n";
	}
	
	s.set(RECAP);
	s.recap.scores[1] = 456;
	s.recap.scores[456] = 45600;
	
	ss = s;
	
	for (auto i: ss.recap.scores) {
		cout << "ss.scores: " << i.first << " --> " << i.second << "\n";
	}
	
	cout << (s == ss) << "\n";
}

#endif
