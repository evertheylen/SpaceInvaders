
#include "cmd.hpp"

#include "view/sfml/sfmlview.hpp"
#include "controller/sfml/sfmlcontroller.hpp"
#include "viewcontroller/sfml/sfmlvc.hpp"

using namespace si;
using namespace si::util;

void CmdResult::add_view(view::View* v) {
	views.push_back(std::move(std::unique_ptr<view::View>(v)));
}

void CmdResult::add_controller(controller::Controller* c) {
	controllers.push_back(std::move(std::unique_ptr<controller::Controller>(c)));
}

void CmdResult::add_vc(vc::ViewController* handle) {
	vcs.push_back(std::move(std::unique_ptr<vc::ViewController>(handle)));
}

CmdParser::CmdParser(std::ostream& _out):
	out(_out) {}


void CmdParser::help() {
	out << "\n";
	out << "  S P A C E     I N V A D E R S\n";
	out << "  -----------------------------\n\n";
	
	out << "                by Evert Heylen\n\n";
	
	out << "Usage:\n";
	out << "  \033[1mspaceinvaders <TOML file> <VC>+\033[0m\n";
	out << "  Where VC is a string denoting a View, Controller or both.\n";
	out << "  The format is: (WEB|SFML):(V|VC)[:option]\n\n";
	
	out << "Examples:\n";
	out << "  - Start a simple game with one SFML View and Controller:\n";
	out << "    \033[1mspaceinvaders file.toml SFML:VC\033[0m\n";
	out << "  - Start a multiplayer game with one player through SFML and another through web.\n";
	out << "    \033[1mspaceinvaders file.toml SFML:VC WEB:VC\033[0m\n";
	out << "  - Start a multiplayer game with 3 players through web on different ports, and one SFML screen to watch.\n";
	out << "    \033[1mspaceinvaders file.toml WEB:VC:8080 WEB:VC:8081 WEB:VC:8082 SFML:V\033[0m\n\n";
	
	out << "Have fun!\n";
}

CmdResult CmdParser::parse(std::vector<std::string>& args, Game* g) {
	CmdResult result;
	for (std::string& arg: args) {
		std::vector<std::string> parts = split(arg, ':');
		if (parts.size() < 2 or parts.size() > 3) throw CmdError(arg, "wrong number of parts");
		
		if (parts[0] == "SFML") {
			if (parts[1] == "V" or parts[1] == "VC") {
				bool concurrent = false;
				if (parts.size() == 3 && parts[2] == "cc") {
					concurrent = true;
					out << "Using a concurrent SfmlView\n";
				}
				view::SfmlView* v = new view::SfmlView(g, concurrent);
				vc::SfmlVc* handle = new vc::SfmlVc();
				handle->couple_view(v);
				result.add_view(v);
				result.add_vc(handle);
				if (parts[1] == "VC") {
					controller::SfmlController* c = new controller::SfmlController(g, concurrent);
					handle->couple_controller(c);
					result.add_controller(c);
				}
			} else {
				throw CmdError(arg, "Invalid VC/V configuration. You can only use VC or V.\n");
			}
		} else {
			throw CmdError(arg, "I don't know about that View/Controller.");
		}
	}
	return result;
}

