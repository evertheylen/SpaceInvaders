
#include <thread>
#include <iostream>
#include <vector>
#include <chrono>
#include <mutex>
#include <iomanip>
#include <random>

#include <pthread.h>
#include <atomic>

using ClockT = std::chrono::high_resolution_clock;
using TimepointT = std::chrono::time_point<ClockT>;

// Did you know C++ has the thread_local keyword?
// neither did I.
// This is only used for readers, which is a fair price to pay (there's only one writer usually)
thread_local TimepointT tsd_last_update;

class RWLock {
public:
	RWLock() {
		lock = new pthread_rwlock_t;
		pthread_rwlock_init(lock, nullptr);
	}
	
	bool read_lock() {
		if (closed) return false;
		while (true) { // spinning all day, all night
			if (tsd_last_update < update.load()) {
				pthread_rwlock_rdlock(lock);
				return true;
			}
		}
	}
	
	bool try_read_lock() {
		if (closed) return false;
		if (tsd_last_update < update.load()) {
			return pthread_rwlock_tryrdlock(lock);
		} else {
			return false;
		}
	}
	
	bool write_lock() {
		if (closed) return false;
		pthread_rwlock_wrlock(lock);
	}
	
	void write_unlock() {
		update = clock.now();
		pthread_rwlock_unlock(lock);
	}
	
	void read_unlock() {
		tsd_last_update = clock.now();
		pthread_rwlock_unlock(lock);
	}
	
	void close() {
		closed = true;
	}
	
	~RWLock() {
		delete lock;
	}
	
private:
	static ClockT clock;
	
	std::atomic<TimepointT> update{TimepointT()};
	std::atomic<bool> closed{false};
	pthread_rwlock_t* lock = nullptr;
	//static pthread_rwlockattr_t attr;
};

/* timeline

r      |--------|         |---------|
r      |----|             |-----|
r      |------|           |-------|
w |----|        |---||----|         |----|
  ------------------------------------------------> t

*/

constexpr int num = 50;

int main() {
	int data = 0;
	RWLock rw;
	
	std::random_device rd;
	std::uniform_real_distribution<> dist;
	
	std::mutex cout_lock;
	auto print = [&](const char* s) {
		cout_lock.lock();
		std::cout << s;
		cout_lock.unlock();
	};
	
	auto printi = [&](int s) {
		cout_lock.lock();
		std::cout << s;
		cout_lock.unlock();
	};
	
	auto reader = [&](int ID) {
		print("READ STARTED\n");
		for (int i=0; i<num; i++) {
			if (rw.read_lock()) {
				//rw.read_lock();
				print("read\n");
				std::this_thread::sleep_for(std::chrono::microseconds(int(dist(rd)*5000.0)));
				rw.read_unlock();
			} else {
				print("rwlock closed\n");
			}
		}
		std::cout << "last_update:" << tsd_last_update.time_since_epoch().count() << "\n";
	};
	
	auto writer = [&]() {
		print("WRITE STARTED\n");
		for (int i=0; i<num; i++) {
			rw.write_lock();
			print("----- write\n");
			std::this_thread::sleep_for(std::chrono::microseconds(int(dist(rd)*5000.0)));
			data++;
			rw.write_unlock();
		}
		rw.close();
	};
	
	std::vector<std::thread*> threads;
	for (int i=0; i<5; i++) {
		threads.push_back(new std::thread(reader, i));
	}
	threads.push_back(new std::thread(writer));
	
	for (auto t: threads) {
		t->join();
		delete t;
	}
	
	return 0;
}

