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

		void OnMouseMoved(const sf::Event::MouseMoveEvent& event);

	protected:
		void Init();

		std::unique_ptr<View> m_mainView;
		std::vector<View*> m_views;
	};
}