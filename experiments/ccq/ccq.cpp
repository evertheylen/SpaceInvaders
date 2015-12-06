
#include <mutex>
#include <thread>
#include <queue>

#include <cstdlib>
#include <iostream>
#include <chrono>

// Assumes only one thread can pop (consumer?), multiple threads can push (producers?).
template <typename T>
class CCQueue {
public:
	// enqueue a value
	void push(const T& el) {
		lock.lock();
		data.push(el);
		lock.unlock();
	}
	
	// usually empty() is called like this:
	//   if (not q.empty()) val = q.pop();
	// This could be problematic if another thread pops a value between the empty() and deque()
	// calls. However, we assume there is only one thread popping. The only thing that could
	// happen is another thread pushing a value, which is not a problem.
	bool empty() {
		return data.empty();
	}
	
	// dequeues and returns the value on the begin of the queue
	T pop() {
		T temp;
		lock.lock();
		temp = data.front();
		data.pop();
		lock.unlock();
		return temp;
	}

private:
	// a mutex to make enqueue-ing and dequeue-ing atomic operations
	std::mutex lock;
	
	// a queue to save the values in
	// by default uses a deque as Container
	std::queue<T> data;
};


int main() {
	CCQueue<int> q;
	auto producer = [&]() {
		for (int i=0; i<500; i++) {
			q.push(i);
		}
	};
	
	auto consumer = [&]() {
		int total = 0;
		while (true) while (not q.empty()) {
			int i = q.pop();
			total += i;
			if (i==-1) {
				std::cout << total << "\n";
				return;
			}
		}
	};
	
	std::thread t(consumer);
	
	for (int i=0; i<50; i++) {
		new std::thread(producer);
	}
	
	std::this_thread::sleep_for(std::chrono::seconds(1));
	q.push(-1);
	t.join();
}
