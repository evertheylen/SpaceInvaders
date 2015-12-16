
// Main executable for Space Invaders

/* [bake me]

dependencies["build_exec"] = [
	"game>>build_objects",
	"view/sfml>>build_objects",
	"viewcontroller>>build_objects",
	"util/cmd>>build_objects",
	# "libs/yomm11>cmake>all",
	# not mentioning sfml, CMake takes too long to actually realize it doesn't have to do anything
]

# SFML instructions:
# in libs/sfml/cmake_stuff/, execute:
#   cmake .. -DBUILD_SHARED_LIBS=OFF

gcc_config = {
	"post_extra": "-Wall -Wno-unused-value -Wl,-Bstatic -I libs/yomm11/include -I libs/sfml/include/ "
				+ "-Llibs/yomm11/cmake_stuff/src/ -lyomm11 "
				+ "-Llibs/sfml/cmake_stuff/lib/ -lsfml-graphics-s -lsfml-window-s -lsfml-system-s "
				+ "-Wl,-Bdynamic -lX11 -lX11-xcb -lxcb -lxcb-glx -lxcb-randr -lxcb-icccm -lxcb-image -ludev -lpthread "
				+ "-lGL -lGLEW -lfreetype -ljpeg -lsndfile -lopenal "
}

executable = "spaceinvaders"

[stop baking] */

#include <iostream>
#include <vector>
#include <fstream>

#include "libs/picojson.hpp"

#include "util/cmd/cmd.hpp"
#include "game/game.hpp"
#include "view/sfml/sfmlview.hpp"
#include "view/sfml/sfmlresources.hpp"
#include "controller/sfml/sfmlcontroller.hpp"
#include "model/model.hpp"
#include "viewcontroller/sfml/sfmlvc.hpp"

// so far support for multiple platforms :'(
#include "X11/Xlib.h"

using namespace si;
using namespace si::model;
using namespace si::view;
using namespace si::controller;
using namespace si::vc;
using namespace si::util;

constexpr int numSfmlVcs = 2;

int main(int argc, char** argv) {
	// TODO catch exceptions
	
	// convert to more civil types
	std::vector<std::string> arguments;
	for (int i=1; i<argc; i++) {
		arguments.push_back(std::string(argv[i]));
	}
	
	CmdParser cmd(std::cout);
	
	if (arguments.size() <= 1) {
		cmd.help();
	} else {
		// ----- Init some stuff -----
		// Initialize awesome multimethods library!
		yorel::multi_methods::initialize();
		// multithreaded X11 stuff!
		XInitThreads();
		// Load resources for SFML
		// TODO check for SFML VC's first?
		SfmlView::load_resources();
		
		std::ifstream file;
		file.open(arguments[0].c_str());
		if (not file.is_open()) {
			std::cout << "File '" << arguments[0] << "' is not valid.\n";
			return 1;
		}
		picojson::value v;
		picojson::parse(v, file);
		
		arguments.erase(arguments.begin());
		
		Game g(v);
		
		CmdResult r = cmd.parse(arguments, &g);
		
		// register all views and controller in the game
		for (auto& v: r.views) g.registerView(v.get());
		for (auto& c: r.controllers) g.registerController(c.get());
		
		g.run(); // runs until all views/controllers are done
		
		// cleanup:
		SfmlView::unload_resources();
	}
	
	return 0;
}


