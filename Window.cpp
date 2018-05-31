#include "Window.h"
#include "MainView.h"

#include "SFML/Graphics.hpp"

#include <iostream>

using namespace GMT;

Window::Window() : sf::RenderWindow(sf::VideoMode(), "GMT", sf::Style::Default, sf::ContextSettings())
{
	Init();
}

Window::Window(sf::WindowHandle handle) : sf::RenderWindow(handle)
{
	Init();
}

Window::~Window() = default;

void Window::Init()
{
	m_mainView = std::make_unique<MainView>();
	m_views.push_back(m_mainView.get());
}

void Window::Draw()
{
	for (auto& view : m_views)
	{
		view->SetOrigin({});
		view->BaseDraw(*this);
	}

	display();
}

void Window::OnMouseMoved(const sf::Event::MouseMoveEvent& event)
{
	for (auto& view : m_views)
		if (View* hit = view->HitTest({ event.x, event.y }))
			std::cout << hit << std::endl;
}