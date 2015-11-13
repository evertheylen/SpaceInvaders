
#pragma once

namespace si {
namespace util {

enum Direction {
	NORTH, EAST, SOUTH, WEST
};

void perform(const Direction& dir, double& x, double& y, double amount);


}
}
