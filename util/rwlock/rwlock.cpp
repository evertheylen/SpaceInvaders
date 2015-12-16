
#include "rwlock.hpp"

thread_local TimepointT tsd_last_update;

RWLock::RWLock() {
	lock = new pthread_rwlock_t; // default attributes
	pthread_rwlock_init(lock, nullptr);
}

// if this returns false, the lock has been closed.
bool RWLock::read_lock() {
	if (closed) return false;
	while (true) { // spinning all day, all night
		if (tsd_last_update < update.load()) {
			pthread_rwlock_rdlock(lock);
			return true;
		}
	}
}

bool RWLock::try_read_lock() {
	if (closed) return false;
	if (tsd_last_update < update.load()) {
		return pthread_rwlock_tryrdlock(lock);
	} else {
		return false;
	}
}

bool RWLock::write_lock() {
	if (closed) return false;
	pthread_rwlock_wrlock(lock);
	return true;
}

void RWLock::write_unlock() {
	update = clock.now();
	pthread_rwlock_unlock(lock);
}

void RWLock::read_unlock() {
	tsd_last_update = clock.now();
	pthread_rwlock_unlock(lock);
}

void RWLock::close() {
	closed = true;
}

RWLock::~RWLock() {
	delete lock;
}
