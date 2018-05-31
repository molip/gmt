#pragma once

#include "SFML/Graphics/Rect.hpp"
#include "SFML/Graphics/View.hpp"
#include "SFML/Window/Event.hpp"

#include <memory>
#include <vector>

namespace sf
{
	class RenderWindow;
}
namespace GMT
{
	class View
	{
	public:
		View();
		virtual ~View();

		void SetRect(const sf::IntRect& rect);
		const sf::IntRect& GetRect() const { return m_rect; }

		void SetOrigin(const sf::Vector2i& origin);

		void BaseDraw(sf::RenderWindow& window) const;
		sf::View GetSFView(sf::RenderWindow& window) const;

		View* HitTest(const sf::Vector2i& parentPoint);

		virtual void OnMouseMoved(const sf::Vector2f position) {}

	protected:
		virtual sf::FloatRect GetLogRect() const;
		virtual sf::FloatRect GetClipRect() const;
		virtual void UpdateLayout(const sf::FloatRect logRect) {}
		virtual void Draw(sf::RenderWindow& window) const {}

		sf::IntRect m_rect; // Parent space.
		sf::Vector2i m_origin; // World space.
		std::vector<std::unique_ptr<View>> m_children;
	};
}