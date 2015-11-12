
/* [bake me]

dependencies["headers"] = [
]

[stop baking] */

#pragma once

namespace si {
namespace controller {

class Controller {
public:
	// nothing here yet
	// but it should be virtual!
	void virtual start() = 0;
	
	virtual ~Controller() {};
};

}
}
