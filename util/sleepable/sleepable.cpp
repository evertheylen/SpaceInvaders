
#include "sleepable.hpp"

using namespace si::util;

void Sleepable::sleep() {
	std::unique_lock<std::mutex> locker(sleep_lock);
	sleep_cv.wait(locker);
}

void Sleepable::wake_up() {
	std::unique_lock<std::mutex> locker(sleep_lock);
	sleep_cv.notify_one();
}



