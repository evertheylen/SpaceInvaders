
#pragma once

#include <random>

namespace si {
namespace util {

class RandomGenerator {
public:
	RandomGenerator();
	
	double real(double start = 0, double end = 1.0);
	
	double integer(int start, int end);
	
	// flip a coin :)
	bool flip(double chance = 0.5);
	
	static RandomGenerator* instance();

private:
	std::mt19937 mt;
	
};

// because my program is multithreaded, I create one randomgenerator per thread
extern thread_local RandomGenerator* rg_instance;

}
}
