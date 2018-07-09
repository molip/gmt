#include "ImageObject.h"

#include <SFML/Graphics.hpp>

using namespace GMT;
using namespace GMT::Model;

REGISTER_DYNAMIC(ImageObject)

ImageObject::ImageObject()
{
	m_texture = std::make_unique<sf::Texture>();
	m_texture->loadFromFile("map.jpg");
}

ImageObject::~ImageObject() = default;

void ImageObject::Draw(RenderContext& rc) const
{
	rc.GetWindow().draw(sf::Sprite(*m_texture));
}