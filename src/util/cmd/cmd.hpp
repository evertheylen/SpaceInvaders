
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <utility>

#include "view/view.hpp"
#include "controller/controller.hpp"
#include "viewcontroller/viewcontroller.hpp"
#include "exceptions/exceptions.hpp"


namespace si {

class Game;

namespace util {

struct CmdResult {
	std::vector<std::unique_ptr<view::View>> views;
	std::vector<std::unique_ptr<controller::Controller>> controllers;
	std::vector<std::unique_ptr<vc::ViewController>> vcs;
	
	void add_view(view::View* v);
	void add_controller(controller::Controller* c);
	void add_vc(vc::ViewController* handle);
};

class CmdParser {
public:
	CmdParser(std::ostream& out=std::cout);
	
	void help();
	
	// could fail, throws exception in that case
	CmdResult parse(std::vector<std::string>& args, Game* g);
	
private:
	std::ostream& out;
};

}
}
