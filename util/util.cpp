
#include "util.hpp"
#include <iostream>

using namespace si::util;

void si::util::perform(const Direction& dir, double& x, double& y, double amount) {
	// in the coordinate system of SFML that is (left top is (0,0))
	std::cout << " in y = " << y << ", amout = " << amount << "\n";
	switch (dir) {
		case Direction::NORTH:
			y += 10;
			break;
		case Direction::EAST:
			std::cout << "EAST\n";
			x += amount;
			break;
		case Direction::SOUTH:
			y -= amount;
			break;
		case Direction::WEST:
			x -= amount;
			break;
	}
	std::cout << " out y = " << y << "\n";
}
