
// Main executable for Space Invaders

/* [bake me]

dependencies["build_exec"] = [
	"game>>build_objects",
	"controller/sfml>>build_objects",
	"view/sfml>>build_objects",
	"viewcontroller/sfml>>build_objects",
	"libs/yomm11>cmake>all",
	# not mentioning sfml, CMake takes too long to actually realize it doesn't have to do anything
]

gcc_config = {
	"post_extra": "-Wall -Wl,-Bstatic -I libs/yomm11/include -I libs/sfml/include/ "
				+ "-Llibs/sfml/cmake_stuff/lib/ -lsfml-graphics-s-d -lsfml-window-s-d -lsfml-system-s-d "
				+ "-Llibs/yomm11/cmake_stuff/src/ -lyomm11 "
				+ "-Wl,-Bdynamic -lX11 -lX11-xcb -lxcb -lxcb-glx -lxcb-randr -lxcb-icccm -lxcb-image -ludev -lpthread "
				+ "-lGL -lGLEW -lfreetype -ljpeg -lsndfile -lopenal "
}

executable = "spaceinvaders"

[stop baking] */

#include <iostream>
#include <vector>
#include <fstream>

#include "libs/picojson.hpp"

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
	
	if (arguments.size() <= 1) {
		std::cout << "\n";
		std::cout << "  S P A C E     I N V A D E R S\n";
		std::cout << "  -----------------------------\n\n";
		
		std::cout << "                by Evert Heylen\n\n";
		
		std::cout << "Usage:\n";
		std::cout << "  \033[1mspaceinvaders <TOML file> <VC>+\033[0m\n";
		std::cout << "  Where VC is a string denoting a View, Controller or both.\n";
		std::cout << "  The format is: (WEB|SFML):(V|C|VC)[:option]\n\n";
		
		std::cout << "Examples:\n";
		std::cout << "  - Start a simple game with one SFML View and Controller:\n";
		std::cout << "    \033[1mspaceinvaders file.toml SFML:VC\033[0m\n";
		std::cout << "  - Start a multiplayer game with one player through SFML and another through web.\n";
		std::cout << "    \033[1mspaceinvaders file.toml SFML:VC WEB:VC\033[0m\n";
		std::cout << "  - Start a multiplayer game with 3 players through web on different ports, and one SFML screen to watch.\n";
		std::cout << "    \033[1mspaceinvaders file.toml WEB:VC:8080 WEB:VC:8081 WEB:VC:8082 SFML:V\033[0m\n\n";
		
		std::cout << "Have fun!\n";
	} else {
		// multithreading FTW
		XInitThreads();
		
		// Load resources for SFML
		SfmlView::load_resources();
		
		std::ifstream file;
		file.open(arguments[0]);
		picojson::value v;
		picojson::parse(v, file);
		
		Game g(v);
		
		std::vector<SfmlView*> views;
		views.reserve(numSfmlVcs);
		std::vector<SfmlController*> controllers;
		controllers.reserve(numSfmlVcs);
		std::vector<SfmlVc*> handles;
		handles.reserve(numSfmlVcs);
		for (int i=0; i<numSfmlVcs; i++) {
			views.emplace_back(new SfmlView(&g));
			controllers.emplace_back(new SfmlController(&g));
			handles.emplace_back(new SfmlVc()); // at this point the SfmlWindow is created
			handles[i]->couple_view(views[i]);
			handles[i]->couple_controller(controllers[i]);
			
			// dangerous :)
			// only works because I reserved enough place
			g.registerView(views[i]);
			g.registerController(controllers[i]);
		}
		
		g.run(); // runs until all views/controllers are done
		
		// cleanup:
		SfmlView::unload_resources();
	}
	
	return 0;
}


