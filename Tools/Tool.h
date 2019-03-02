#pragma once

#include "HitTester.h"

#include "../RenderContext.h"
#include "../Model/VectorObject.h"
#include "../Model/Element.h"

#include <SFML/System.hpp>
#include <SFML/Window.hpp>

namespace GMT
{
	class MainView;
}

namespace GMT::Tools
{
	class Tool
	{
	public:
		Tool(const MainView& view);
		virtual ~Tool();

		virtual void Draw(RenderContext& rc) const = 0;
		virtual void OnMouseMoved(const sf::Vector2i& point) {};
		virtual void OnMouseDown(sf::Mouse::Button button, const sf::Vector2i& point) {}
		virtual void OnMouseUp(sf::Mouse::Button button, const sf::Vector2i& point) {}
		virtual void OnKeyPressed(const sf::Event::KeyEvent event) {}
		virtual void Update() {}

	protected:
		Model::ElementPtr HitTest(const sf::Vector2f& logPoint, const Model::VectorObject* special, const HitTester::Options& opts) const;

		const MainView& m_view;
	};
}
