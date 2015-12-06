/* [bake me]

dependencies["build_exec"] = [
]

executable = "input_test"

[stop baking] */

#include <iostream>
#include <chrono>

#include "SFML/Graphics.hpp"


int main() {
	sf::Window window(sf::VideoMode(200, 200), "Space Invaders");
	
	std::chrono::system_clock::time_point start;
	std::chrono::system_clock::time_point end;
	bool pressed = false;
	
	while (true) {
		sf::Event event;
		while (window.waitEvent(event)) {
			switch (event.type) {
				case sf::Event::Closed: {
					std::cout << "SfmlVc got close Event\n";
					window.close();
					exit(0);
				}
				case sf::Event::KeyPressed: {
					if (not pressed) {
						start = std::chrono::system_clock::now();
						pressed = true;
					}
					break;
				}
				case sf::Event::KeyReleased: {
					if (pressed) {
						end = std::chrono::system_clock::now();
						pressed = false;
						std::chrono::system_clock::duration time_elapsed = end-start;
						std::cout << std::chrono::duration_cast<std::chrono::microseconds>(time_elapsed).count() << " microseconds\n";
					}
					break;
				}
				default: break;
			}
			
		}
	}
}
