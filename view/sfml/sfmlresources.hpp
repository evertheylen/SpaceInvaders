
#include <cassert>

#include "SFML/Graphics.hpp"

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
		
		assert(bullet.loadFromFile("data/textures/bullet.png"));
		
		assert(font.loadFromFile("data/fonts/roboto.ttf"));
		
		assert(background.loadFromFile("data/textures/background.png"));
	}
	
	sf::Texture player;
	sf::Texture small_alien;
	sf::Texture medium_alien;
	sf::Texture big_alien;
	
	sf::Texture bullet;
	
	sf::Texture small_bomb;
	sf::Texture medium_bomb;
	sf::Texture big_bomb;
	
	sf::Font font;
	
	sf::Texture background;
};

}
}
