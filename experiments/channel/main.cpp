
/* [bake me]

dependencies["build_exec"] = [
	"libs/channel>>build_objects",
]

executable = "channel_test"

[stop baking] */

#include <thread>
#include <chrono>
#include <iostream>

//#include "libs/channel/channel.h"
//using namespace ch;

#include <vector>
#include <mutex>
#include <algorithm>
#include <cassert>

template <typename T, typename size_t=unsigned int>
class channel {
public:
	
	channel(size_t buf_cap = 0):
			buffer_cap(buf_cap) {
		buffer.reserve(buf_cap == 0 ? 1 : buf_cap);
	}
	
	void send(const T& data) {
		// basically, two options:
		//   - we can provide data
		//   - we have to wait until we provide a value
		// if the buffer is full (always full if synchronous), we have to wait.
		std::cerr << "[send: entered send]\n";
		buffer_lock.lock();
		std::cerr << "[send: got buffer lock]\n";
		if (buffer_has_place()) {
			std::cerr << "[send: buffer has place]\n";
			// we can in fact provide a value
			buffer_push(data);
			std::cerr << "[send: pushed data]\n";
			buffer_lock.unlock();
			std::cerr << "[send: unlocked buffer]\n";
			// it is possible someone was waiting for data to arrive, notify (one of) them.
			needs_value.unlock();
			std::cerr << "[send: unlocked needs_value]\n";
		} else {
			// we have to wait until we provide a value
			buffer_lock.unlock();
			std::cerr << "[send: unlocked buffer]\n";
			provides_value.lock();
			std::cerr << "[send: single lock provides_value]\n";
			provides_value.lock();
			
			std::cerr << "[send: double lock unlocked]\n";
			// unlocked by receiver
			buffer_lock.lock();
			std::cerr << "[send: got buffer lock]\n";
			assert(buffer_has_place() or (buffer_cap == 0));
			// we can in fact provide a value
			buffer_push(data);
			std::cerr << "[send: pushed data]\n";
			buffer_lock.unlock();
			std::cerr << "[send: unlocked buffer]\n";
			// unlock the double lock
			provides_value.unlock();
			std::cerr << "[send: unlocked double lock]\n";
			
			// it is possible someone was waiting for data to arrive, notify (one of) them.
			needs_value.unlock();
			std::cerr << "[send: unlocked needs_value]\n";
		}
	}
	
	T recv() {
		// two options again
		//   - there is data available
		//   - we have to wait until there is data available
		std::cerr << "  [recv: entered recv]\n";
		buffer_lock.lock();
		std::cerr << "  [recv: got buffer]\n";
		if (buffer_has_data()) {
			std::cerr << "  [recv: buffer has data]\n";
			// we can in fact just get data from the buffer
			T data = buffer_pop();
			buffer_lock.unlock();
			std::cerr << "  [recv: unlocked buffer]\n";
			// it is possible someone was trying to send data, notify (one of) them.
			provides_value.unlock();
			std::cerr << "  [recv: unlocked provides_value]\n";
			return data;
		} else {
			// we need to wait until someone sends us data
			buffer_lock.unlock();
			std::cerr << "  [recv: unlocked buffer]\n";
			needs_value.lock();
			std::cerr << "  [recv: single lock needs_value]\n";
			needs_value.lock(); // double lock
			
			std::cerr << "  [recv: double lock unlocked]\n";
			// unlock by a sender
			buffer_lock.lock();
			std::cerr << "  [recv: got buffer lock]\n";
			assert(buffer_has_data());
			T data = buffer_pop();
			std::cerr << "  [recv: popped data]\n";
			buffer_lock.unlock();
			std::cerr << "  [recv: unlocked buffer]\n";
			// unlock the double lock
			needs_value.unlock();
			std::cerr << "  [recv: unlocked double lock]\n";
			
			// it is possible someone was trying to send data
			provides_value.unlock();
			std::cerr << "  [recv: unlocked provides_value]\n";
			return data;
		}
	}
	
	bool buffer_has_place() {
		return buffer.size() <= buffer_cap;
	}
	
	bool buffer_has_data() {
		return buffer.size() > 0;
	}
	
	void buffer_push(const T& data) {
		buffer.push_back(data);
	}
	
	T buffer_pop() {
		T data(buffer.at(0));
		buffer.erase(buffer.begin());
		return data;
	}
	
	
	std::mutex needs_value;
	
	std::mutex provides_value;
	
	std::mutex buffer_lock;
	
	size_t buffer_cap; // if 0, buffer is still of capacity 1
	std::vector<T> buffer; // can be empty
};

using intchannel = channel<int>;

/*

void model_loop(channel<int> c) {
	int answer = 0;
	do {
		answer = c.recv();
		//std::cout << "Answer is " << answer << "\n";
	} while (answer != 0);
}

#define sleep(i) std::this_thread::sleep_for(std::chrono::milliseconds(i))
#define send(msg) std::cout << "sending " << msg << std::endl; c.send(msg)

void controller_loop(channel<int> c) {
	for (int i=0; i<10; i++) {
		send(2);
		send(8);
		send(80);
	}
}

*/


/*

int main() {
	channel<int> c;
	std::thread controller1(controller_loop, c);
	std::thread controller2(controller_loop, c);
	//sleep(500);
	std::thread model(model_loop, c);
	
	controller1.join();
	controller2.join();
	send(0); // quit
	model.join();
	
}

*/

void send_randoms(int amount, channel<int>& c) {
	for (int i=0; i<amount; i++) {
		c.send(1);
	}
}

void get_randoms(int amount, channel<int>& c) {
	int total = 0;
	for (int i=0; i<amount-1; i++) {
		total += c.recv();
	}
	std::cout << "[" << std::this_thread::get_id() << "] total is " << total << "\n";
}

int main() {
	int num = 1;
	int amount = 5;
	channel<int> c;
	std::vector<std::thread*> v;
	for (int i; i<num; i++) {
		v.push_back(new std::thread(send_randoms, amount, std::ref(c)));
		v.push_back(new std::thread(get_randoms, amount, std::ref(c)));
	}
	
	for (std::thread* p: v) {
		p->join();
	}
}


/*
// manual test
int main() {
	channel<int> c(4);
	std::thread print([&]() {
		while (true) {
			int val = c.recv();
			int val2 = c.recv();
			std::cout << ">>>" << val << ", " << val2 << "\n";
			if (val == 0) break;
		}
	});
	
	int input;
	do {
		std::cout << "? ";
		std::cin >> input;
		c.send(input);
		c.send(input);
		c.send(input);
		
	} while (input != 0);
	print.join();
}

*/



