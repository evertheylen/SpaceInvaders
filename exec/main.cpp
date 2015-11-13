
// Main executable for Space Invaders

/* [bake me]

dependencies["build_exec"] = [
	"game>>build_objects",
	"controller/sfml>>build_objects",
	"view/sfml>>build_objects",
	"viewcontroller/sfml>>build_objects",
	"libs/tinyxml>>build_objects",
]

executable = "spaceinvaders"

[stop baking] */

#include <iostream>
#include <vector>

#include "libs/tinyxml/tinyxml.h"

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

const int numSfmlVcs = 10;

int main(int argc, char** argv) {
	// multithreading FTW
	XInitThreads();
	
	// Load resources for SFML
	SfmlView::load_resources();
	
	TiXmlDocument doc;
	
	Game g(doc);
	
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
	
	return 0;
}


