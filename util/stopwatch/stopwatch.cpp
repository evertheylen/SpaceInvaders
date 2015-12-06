
#include "stopwatch.hpp"

using namespace si::util;

Stopwatch::TimePoint Stopwatch::now() {
	return c.now();
}
