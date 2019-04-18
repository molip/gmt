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

		View& AddChild(std::unique_ptr<View> child);

		void Arrange() { Arrange(m_rect); }
		virtual void Arrange(const sf::IntRect& rect);
		virtual sf::Vector2i GetIdealSize(const sf::Vector2i& available) const { return available; }

		View* GetParent() { return m_parent; }
		const sf::IntRect& GetRect() const { return m_rect; }

		void BaseDraw(sf::RenderWindow& window, const sf::IntRect& parentClip) const;

		View* HitTest(const sf::Vector2i& worldPoint);

		virtual void OnMouseMoved(const sf::Vector2i& point) {}
		virtual void OnMouseDown(sf::Mouse::Button button, const sf::Vector2i& point) {}
		virtual void OnMouseUp(sf::Mouse::Button button, const sf::Vector2i& point) {}
		virtual bool OnMouseWheel(float delta) { return false; }

		virtual void OnKeyPressed(const sf::Event::KeyEvent event) {}

		sf::Vector2i WorldToLocal(const sf::Vector2i& point) const;
		sf::Vector2f DevToLog(const sf::Vector2i& point) const;

	protected:
		sf::FloatRect GetLogRect() const;
		virtual float GetDevToLogScale() const { return 1; }
		virtual void Draw(sf::RenderWindow& window) const {}

		sf::IntRect m_rect; // World space.
		std::vector<std::unique_ptr<View>> m_children;
		View* m_parent;
	};
}