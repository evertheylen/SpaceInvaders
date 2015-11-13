
#include "SFML/Graphics.hpp"

#include <cassert>

#pragma once

namespace si {
namespace view {

class SfmlResources {
public:
	
	SfmlResources() {
		assert(player.loadFromFile("data/textures/player.png"));
		assert(small_alien.loadFromFile("data/textures/small.png"));;
		assert(medium_alien.loadFromFile("data/textures/medium.png"));
		assert(big_alien.loadFromFile("data/textures/big.png"));
		
		assert(background.loadFromFile("data/textures/background.png"));
		
		loaded = true;
	}
	
	bool loaded = false;
	
	sf::Texture player;
	sf::Texture small_alien;
	sf::Texture medium_alien;
	sf::Texture big_alien;
	
	sf::Texture background;
};

}
}
