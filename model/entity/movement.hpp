
#pragma once

#include <chrono>

#include "util/util.hpp"

namespace si {
namespace model {

class Movement {
public:
	Movement() = default;
	
	Movement(double _speed, util::Vector2D_d _dir):
			speed(_speed), dir(_dir) {
		dir.normalize(); // normalize!
	}
	
	template <typename durationT>
	void perform(durationT duration, util::Vector2D_d& vec) {
		if (dir.length() != 0) {
			vec += (dir * (speed*std::chrono::duration_cast<std::chrono::microseconds>(duration).count()));
		}
	}
	
	double speed = 0.0;                // per millisecond
	util::Vector2D_d dir = util::HOLD; // should always have length 1, unless length 0
};



}
}
