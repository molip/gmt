#pragma once

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Window/Event.hpp"

#include <memory>
#include <vector>

namespace GMT
{
	class View;

	class Window : public sf::RenderWindow
	{
	public:
		Window();
		Window(sf::WindowHandle handle);
		virtual ~Window();

		void Draw();

		void PumpEvents();

	protected:
		void Init();

		void OnMouseMoved(const sf::Event::MouseMoveEvent& event);
		void OnMouseDown(const sf::Event::MouseButtonEvent& event);
		void OnMouseUp(const sf::Event::MouseButtonEvent& event);

		View* HitTest(const sf::Vector2i& point);

		std::unique_ptr<View> m_mainView;
		std::vector<View*> m_views;
	};
}