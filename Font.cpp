#include "Font.h"

#include "SFML/Graphics/Font.hpp"

using namespace GMT; 

std::unique_ptr<sf::Font> Font::s_font;

sf::Font& Font::Get()
{
	if (s_font == nullptr)
	{
		s_font = std::make_unique<sf::Font>();
		s_font->loadFromFile("C:/Windows/Fonts/Tahoma.ttf");
	}
	return *s_font;
}
