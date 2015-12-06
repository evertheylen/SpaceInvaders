
#pragma once

#include <mutex>
#include <thread>
#include <queue>

namespace si {
namespace util {

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


}
}
