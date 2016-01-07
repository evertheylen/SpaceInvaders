
#pragma once

#include <mutex>
#include <condition_variable>

namespace si {
namespace util {

// partly based on http://stackoverflow.com/a/14921115/2678118
// Implements a simple boolean flag
class Flag {
public:
	bool is_set() const {
		std::lock_guard<std::mutex> lock(m);
		return flag;
	}

	void set() {
		{
			std::lock_guard<std::mutex> lock(m);
			flag = true;
		}
		cv.notify_all();
	}

	void reset() {
		{
			std::lock_guard<std::mutex> lock(m);
			flag = false;
		}
		cv.notify_all();
	}

	void wait() const {
		std::unique_lock<std::mutex> lock(m);
		if (flag) return;
		cv.wait(lock, [this]{ return flag; });
	}

private:
	mutable std::mutex m;
	mutable std::condition_variable cv;
	bool flag = false;
};

}
}
