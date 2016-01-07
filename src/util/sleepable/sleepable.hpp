
#pragma once

#include <mutex>
#include <condition_variable>


namespace si {
namespace util {

class Sleepable {
public:
	void sleep();
	void wake_up();
	
private:
	// to go to sleep / wake up
	// spurious wakeups may happen
	std::mutex sleep_lock;
	std::condition_variable sleep_cv;
};

}
}
