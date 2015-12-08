
#pragma once

#include <chrono>

#include "util/util.hpp"
#include "util/stopwatch/stopwatch.hpp"

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
			//std::cout << typeid(duration).name() << ": " << duration.count() << "\n";
			auto d_count = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(duration).count();
			auto distance = speed*d_count;
			auto mov_vec = dir*distance;
			vec += mov_vec;
		}
	}
	
	double speed = 0.0;                // per millisecond
	util::Vector2D_d dir = util::HOLD; // should always have length 1, unless length 0
};



}
}
