
#pragma once

#include <chrono>

namespace si {
namespace util {

class Stopwatch {
public:
	using Clock = std::chrono::high_resolution_clock;
	using TimePoint = std::chrono::time_point<Clock>;
	
	TimePoint now();
	
private:
	std::chrono::high_resolution_clock c;
};


}
}
