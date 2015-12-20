
#include <iostream>
#include <mutex>

class Guard {
public:
	
	Guard() {
		std::cout << "opening guard\n";
	}
	
	Guard(Guard&& g) {
		std::cout << "passing guard around\n";
		g.has_ownership = false;
	}
	
	Guard(Guard& g) = delete;
	
	Guard& operator=(Guard& g) = delete;
	
	~Guard() {
		if (has_ownership) {
			std::cout << "closing guard\n";
			has_ownership = false;
		}
	}
	
	bool has_ownership = true;
};

Guard make_guard() {
	std::cout << "--- make_guard {\n";
	Guard g;
	std::cout << "--- make_guard }\n";
	return g;
}


void do_something_else(Guard g) {
	std::cout << "doing other stuff...\n";
}

void do_something(Guard g) {
	std::cout << "doing stuff...\n";
	do_something_else(std::move(g));
}


int main() {
	Guard g = std::move(make_guard());
	std::cout << "bla b;a bla\n";
	do_something(std::move(g));
	std::cout << "main is done\n";
	return 0;
}
