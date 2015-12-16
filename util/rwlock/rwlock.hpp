
#pragma once

// C++ does not have a rwlock...
#include <pthread.h>
// ... so we borrow it from C
#include <atomic>
#include <chrono>

// own clock because rwlock should not depend on stopwatch
using ClockT = std::chrono::high_resolution_clock;
using TimepointT = std::chrono::time_point<ClockT>;

// Did you know C++ has the thread_local keyword?
// Neither did I. "tsd" stands for Thread Specific Data.
// This is only used for readers, which is a fair price to pay (there's only one writer usually)
extern thread_local TimepointT tsd_last_update;

class RWLock {
public:
	RWLock();
	
	// if this returns false, the lock has been closed.
	bool read_lock();
	
	bool try_read_lock();
	
	bool write_lock();
	
	void write_unlock();
	
	void read_unlock();
	
	void close();
	
	~RWLock();
	
private:
	static ClockT clock;
	
	std::atomic<TimepointT> update{TimepointT()};
	std::atomic<bool> closed{false};
	pthread_rwlock_t* lock = nullptr;
	//static pthread_rwlockattr_t attr;
};

/* possible timeline:

r      |--------|         |---------|
r      |----|             |-----|
r      |------|           |-------|       
w |----|        |---||----|         |----|
  ------------------------------------------------> t

*/

