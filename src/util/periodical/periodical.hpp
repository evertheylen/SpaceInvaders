
#pragma once

#include <chrono>
#include <vector>

namespace si {
namespace util {

	
class Periodical {
public:
	using DurationT = std::chrono::duration<double, std::milli>;
	
	Periodical(const std::vector<double>& _phases):
			phases(_phases) {}
	
	Periodical(): phases({1}) {}
	
	template <typename T>
	bool advance(const T& extra_dur) {
		dur += extra_dur;
		if (dur >= DurationT(phases.at(current_phase) * modifier)) {
			current_phase = (current_phase+1) % phases.size();
			dur *= 0;
			return true;
		}
		return false;
	}
	
	int get_current_phase() {
		return current_phase;
	}
	
	void set_modifier(double mod) {
		modifier = mod;
	}
	
	double get_modifier() {
		return modifier;
	}
	
	void reset() {
		current_phase = 0;
	}
	
private:
	DurationT dur;
	std::vector<double> phases;
	double modifier = 1.0;
	int current_phase = 0;
};

}
}
