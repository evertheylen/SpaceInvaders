
/* [bake me]

dependencies["headers"] = [
]

[stop baking] */

#pragma once

#include <thread>

namespace si {
namespace controller {

class Controller {
public:
	// nothing here yet
	// but it should be virtual!
	virtual std::thread* start() = 0;
	
	virtual ~Controller() {};
};

}
}
