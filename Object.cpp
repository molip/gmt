#include "Object.h"

#include <SFML/Graphics.hpp>

using namespace GMT;

Object::Object()
{
	m_texture = std::make_unique<sf::Texture>();
	m_texture->loadFromFile("map.jpg");

	m_sprite = std::make_unique<sf::Sprite>(*m_texture);
}

Object::~Object() = default;

void Object::Draw(sf::RenderWindow& window)	const
{
	window.draw(*m_sprite);
}