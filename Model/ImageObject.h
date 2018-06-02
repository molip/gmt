#pragma once

#include "Object.h"

#include <memory>

namespace sf
{
	class Texture;
	class Sprite;
}

namespace GMT::Model
{
	class ImageObject : public Object
	{
	public:
		ImageObject();
		virtual ~ImageObject();

		virtual void Draw(RenderContext& rc) const override;

	private:
		std::unique_ptr<sf::Texture> m_texture;
		std::unique_ptr<sf::Sprite> m_sprite;
	};
}
