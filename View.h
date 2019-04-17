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

		//void Arrange();
		virtual void Arrange(const sf::IntRect& rect);
		const sf::IntRect& GetRect() const { return m_rect; }

		void SetParent(View* parent) { m_parent = parent; }

		void BaseDraw(sf::RenderWindow& window, const sf::FloatRect& clip) const;
		sf::View GetSFView(sf::RenderWindow& window) const;

		View* HitTest(const sf::Vector2i& worldPoint);

		virtual void OnMouseMoved(const sf::Vector2i& point) {}
		virtual void OnMouseDown(sf::Mouse::Button button, const sf::Vector2i& point) {}
		virtual void OnMouseUp(sf::Mouse::Button button, const sf::Vector2i& point) {}

		virtual void OnKeyPressed(const sf::Event::KeyEvent event) {}

		sf::Vector2i WorldToLocal(const sf::Vector2i& point) const;
		sf::Vector2f DevToLog(const sf::Vector2i& point) const;

	protected:
		virtual sf::FloatRect GetLogRect() const;
		virtual sf::FloatRect GetClipRect() const;
		virtual void Draw(sf::RenderWindow& window) const {}

		sf::IntRect m_rect; // World space.
		std::vector<std::unique_ptr<View>> m_children;
		View* m_parent;
	};
}