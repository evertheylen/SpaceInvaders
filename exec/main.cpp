
// Main executable for Space Invaders

/* [bake me]

dependencies["build_exec"] = [
	"game>>build_objects",
	"libs/tinyxml>>build_objects",
]

executable = "spaceinvaders"

[stop baking] */

#include <iostream>

#include "libs/tinyxml/tinyxml.h"
#include "game/game.hpp"

using namespace si;

int main(int argc, char** argv) {
	std::cout << "Hello world\n";
	TiXmlDocument doc;
	Game g(doc);
}


