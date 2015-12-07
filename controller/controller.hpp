
/* [bake me]

dependencies["headers"] = [
]

[stop baking] */

#pragma once

#include <thread>
#include <vector>

namespace si {
namespace controller {

class Controller {
public:
	// nothing here yet
	// but it should be virtual!
	virtual std::vector<std::thread*> start() = 0;
	
	virtual ~Controller() {};
};

}
}
