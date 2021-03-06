#include "Window.h"
#include "App.h"
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

bool Window::TryClose()
{
	if (!App::PreClose())
		return false;

	close();
	return true;
}

void Window::Init()
{
	App::SetMainWindow(this);

	m_mainView = std::make_unique<MainView>();
	m_views.push_back(m_mainView.get());
}

void Window::Draw()
{
	clear();
	
	for (auto& view : m_views)
	{
		view->SetOrigin({});
		view->BaseDraw(*this);
	}

	display();
}

View* Window::HitTest(const sf::Vector2i& point)
{
	for (auto& view : m_views)
		if (View* hit = view->HitTest(point))
			return hit;

	return nullptr;
}

void Window::PumpEvents()
{
	sf::Event event;
	while (pollEvent(event))
		PumpEvent(event);
}

void Window::PumpEvent(const sf::Event& event)
{
	switch (event.type)
	{
	case sf::Event::MouseMoved:
		OnMouseMoved(event.mouseMove);
		break;
	case sf::Event::MouseButtonPressed:
		OnMouseDown(event.mouseButton);
		break;
	case sf::Event::MouseButtonReleased:
		OnMouseUp(event.mouseButton);
		break;
	case sf::Event::KeyPressed:
		m_mainView->OnKeyPressed(event.key);
		break;
	}
}

void Window::OnMouseMoved(const sf::Event::MouseMoveEvent& event)
{
	sf::Vector2i point { event.x, event.y };
	if (View* hit = HitTest(point))
		hit->OnMouseMoved(hit->WorldToLocal(point));
}

void Window::OnMouseDown(const sf::Event::MouseButtonEvent& event)
{
	sf::Vector2i point{ event.x, event.y };
	if (View* hit = HitTest(point))
		hit->OnMouseDown(event.button, hit->WorldToLocal(point));
}

void Window::OnMouseUp(const sf::Event::MouseButtonEvent& event)
{
	sf::Vector2i point{ event.x, event.y };
	if (View* hit = HitTest(point))
		hit->OnMouseUp(event.button, hit->WorldToLocal(point));
}