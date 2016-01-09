
#include "random.hpp"

#include <iostream>

using namespace si::util;

RandomGenerator::RandomGenerator() {
	std::random_device rd;
	mt = std::mt19937(rd());
}

double RandomGenerator::real(double start, double end) {
	std::uniform_real_distribution<double> dist(start, end);
	return dist(mt);
}

double RandomGenerator::integer(int start, int end) {
	std::uniform_int_distribution<int> dist(start, end);
	return dist(mt);;
}

// flip a coin :)
bool RandomGenerator::flip(double chance) {
	return (real() <= chance);
}


thread_local RandomGenerator* si::util::rg_instance = nullptr;

RandomGenerator* RandomGenerator::instance() {
	if (rg_instance == nullptr) {
		rg_instance = new RandomGenerator();
	}
	return rg_instance;
}
