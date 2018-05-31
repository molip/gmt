#pragma once

#include <memory>

namespace sf
{
	class Texture;
	class Image;
	class Sprite;
	class RenderWindow;
}

namespace GMT
{
	class Object
	{
	public:
		Object();
		virtual ~Object();

		void Draw(sf::RenderWindow& window) const;

	private:
		std::unique_ptr<sf::Texture> m_texture;
		std::unique_ptr<sf::Image> m_image;
		std::unique_ptr<sf::Sprite> m_sprite;
	};
}
