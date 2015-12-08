
#pragma once

#include <chrono>

namespace si {
namespace util {

class Stopwatch {
public:
	
	using Clock = std::chrono::high_resolution_clock;
	using Duration = std::chrono::duration<double, std::nano>;
	using TimePoint = std::chrono::time_point<Clock, Duration>;
	
	TimePoint now();
	
private:
	std::chrono::high_resolution_clock c;
};


}
}
