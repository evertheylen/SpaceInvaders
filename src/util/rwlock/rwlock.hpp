
#pragma once

// C++ does not have a rwlock...
#include <pthread.h>
// ... so we borrow it from C
#include <atomic>
#include <chrono>



// Did you know C++ has the thread_local keyword?
// Neither did I. "tsd" stands for Thread Specific Data.
// This is only used for readers, which is a fair price to pay (there's only one writer usually)
// (now replaced with timestamps)
//extern thread_local TimepointT tsd_last_update;

namespace si {
namespace util {

class RWLock {
public:
	// own clock because rwlock should not depend on stopwatch
	using ClockT = std::chrono::high_resolution_clock;
	using TimepointT = std::chrono::time_point<ClockT>;
	
	RWLock();
	
	TimepointT new_timestamp();
	
	// if this returns false, the lock has been closed.
	bool read_lock(TimepointT& stamp);
	
	bool try_read_lock(TimepointT& stamp);
	
	bool write_lock();
	
	void write_unlock();
	
	void read_unlock(TimepointT& stamp);
	
	void close();
	
	~RWLock();
	
private:
	static ClockT clock;
	
	std::atomic<TimepointT> last_write{TimepointT()};
	std::atomic<bool> closed{false};
	
	pthread_rwlock_t* lock = nullptr;
	pthread_rwlockattr_t* attr = nullptr;
};

/* possible timeline:

r      |--------| . . .?  |---------| . ?|------|
r      |----| . . . . . ? |-----| ?      |---|
r      |------| . . . ?   |-------| . ?  |----|  
w |----|        |---||----|         |----|
  ---------------------------------------------------> t

*/

}
}
