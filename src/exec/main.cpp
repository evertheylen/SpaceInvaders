
// Main executable for Space Invaders

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
		// Initialize awesome multimethods library!
		yorel::multi_methods::initialize();
		// multithreaded X11 stuff!
		XInitThreads();
		
		std::ifstream file;
		file.open(arguments[0].c_str());
		if (not file.is_open()) {
			std::cout << "File '" << arguments[0] << "' is not valid.\n";
			return 1;
		}
		picojson::value v;
		picojson::parse(v, file);
		
		arguments.erase(arguments.begin());
		
		// I like to put more whitespace around stuff that is important
		
		Game g(v);
		
		CmdResult r = cmd.parse(arguments, &g);
		
		// Register all views and controller in the game
		// This is like the Observer Pattern
		for (auto& v: r.views) g.register_view(v.get());
		for (auto& c: r.controllers) g.register_controller(c.get());
		
		// starts model, runs until all views/controllers are done, then asks model to stop
		g.run();
	}
	
	return 0;
}


