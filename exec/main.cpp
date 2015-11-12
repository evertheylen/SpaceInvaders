
// Main executable for Space Invaders

/* [bake me]

dependencies["build_exec"] = [
	"game>>build_objects",
	"controller>>build_objects",
	"controller/sfml>>build_objects",
	"view>>build_objects",
	"view/sfml>>build_objects",
	"libs/tinyxml>>build_objects",
]

executable = "spaceinvaders"

[stop baking] */

#include <iostream>

#include "libs/tinyxml/tinyxml.h"

#include "game/game.hpp"
#include "view/sfml/sfmlview.hpp"
#include "controller/sfml/sfmlcontroller.hpp"
#include "model/model.hpp"
#include "viewcontroller/sfml/sfmlvc.hpp"

using namespace si;
using namespace si::model;
using namespace si::view;
using namespace si::controller;
using namespace si::vc;

int main(int argc, char** argv) {
	// Sfml Mode
	std::cout << "Hello world\n";
	TiXmlDocument doc;
	
	Game g(doc);
	
	SfmlView V(&g);
	SfmlController C(&g);
	SfmlVc handle; // at this point the SfmlWindow is created
	handle.couple_view(&V);
	handle.couple_controller(&C);
	
	// JUST LOOK AT IT
	// THIS IS AWESOME
	SfmlView V2(&g);
	SfmlController C2(&g);
	SfmlVc handle2; // at this point the SfmlWindow is created
	handle2.couple_view(&V2);
	handle2.couple_controller(&C2);
	
	g.registerView(&V);
	g.registerController(&C);
	g.registerView(&V2);
	g.registerController(&C2);
	
	g.run(); // runs until all views/controllers are done
}


