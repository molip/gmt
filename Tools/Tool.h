#pragma once

#include "../RenderContext.h"

#include <SFML/System.hpp>
#include <SFML/Window.hpp>

namespace GMT::Tools
{
	class Tool
	{
	public:
		Tool();
		virtual ~Tool();

		virtual void Draw(RenderContext& rc) const = 0;
		virtual void OnMouseMoved(const sf::Vector2i& point) {};
		virtual void OnMouseDown(sf::Mouse::Button button, const sf::Vector2i& point) {}
		virtual void OnMouseUp(sf::Mouse::Button button, const sf::Vector2i& point) {}
		virtual void OnKeyPressed(const sf::Event::KeyEvent event) {}
	};
}
