
#include <thread>
#include <iostream>
#include <vector>
#include <chrono>
#include <mutex>
#include <iomanip>
#include <random>

#include "../../util/rwlock/rwlock.hpp"
#include "../../util/rwlock/rwlock.cpp"

/* timeline

r      |--------|         |---------|
r      |----|             |-----|
r      |------|           |-------|
w |----|        |---||----|         |----|
  ------------------------------------------------> t

*/

constexpr int num = 50;

constexpr int factor = 1;

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
	
	std::atomic<int> writes;
	int maxwrites = 0;
	
	auto reader = [&](int ID) {
		print("READ STARTED\n");
		auto ts = rw.new_timestamp();
		for (int i=0; ; i++) {
			if (rw.read_lock(ts)) {
				writes = 0;
				//rw.read_lock();
				//print("read\n");
				std::this_thread::sleep_for(std::chrono::microseconds(factor*50));
				rw.read_unlock(ts);
			} else {
				print("rwlock closed\n");
			}
		}
		std::cout << "last_update:" << ts.time_since_epoch().count() << "\n";
	};
	
	auto writer = [&]() {
		print("WRITE STARTED\n");
		for (int i=0; ; i++) {
			rw.write_lock();
			//print("----- write\n");
				writes++;
				if (writes > maxwrites) {
					std::cout << "holy fuck, " << writes << " writes!\n";
					maxwrites = writes;
				} 
			std::this_thread::sleep_for(std::chrono::microseconds(factor*1));
			data++;
			rw.write_unlock();
		}
		rw.close();
	};
	
	std::vector<std::thread*> threads;
	for (int i=0; i<1; i++) {
		threads.push_back(new std::thread(reader, i));
	}
	threads.push_back(new std::thread(writer));
	
	for (auto t: threads) {
		t->join();
		delete t;
	}
	
	return 0;
}

