
#include <iostream>

#include "rwlock.hpp"

RWLock::RWLock() {
	lock = new pthread_rwlock_t; // default attributes
	attr = new pthread_rwlockattr_t;
	pthread_rwlockattr_init(attr);
	pthread_rwlockattr_setkind_np(attr, PTHREAD_RWLOCK_PREFER_READER_NP);
	pthread_rwlock_init(lock, attr);
}

TimepointT RWLock::new_timestamp() {
	return TimepointT(); // should be beginning of time
}


// if this returns false, the lock has been closed.
bool RWLock::read_lock(TimepointT& stamp) {
	if (closed) return false;
	while (true) { // spinning all day, all night
		if (stamp < last_write.load()) {
			pthread_rwlock_rdlock(lock);
			return true;
		}
	}
}

bool RWLock::try_read_lock(TimepointT& stamp) {
	if (closed) return false;
	if (stamp < last_write.load()) {
		return pthread_rwlock_tryrdlock(lock);
	} else {
		return false;
	}
}

bool RWLock::write_lock() {
	if (closed) return false;
	pthread_rwlock_wrlock(lock);
	// last_write should kind of be in the future, but if I set it after the lock the
	// readers won't see the difference, and there will be less problems with starvation
	last_write = clock.now();
	return true;
}

void RWLock::write_unlock() {
	pthread_rwlock_unlock(lock);
	// BUG sometimes the readers who are waiting are starved for a significant time
}

void RWLock::read_unlock(TimepointT& stamp) {
	stamp = clock.now();
	pthread_rwlock_unlock(lock);
}

void RWLock::close() {
	closed = true;
}

RWLock::~RWLock() {
	pthread_rwlock_destroy(lock);
	delete lock;
	pthread_rwlockattr_destroy(attr);
	delete attr;
}
