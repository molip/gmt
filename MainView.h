#pragma once

#include "View.h"
#include "Grid.h"

#include <memory>
#include <vector>

namespace GMT
{
	namespace Model
	{
		class Object;
	}

	namespace Tools
	{
		class Tool;
	}

	class MainView : public View
	{
	public:
		MainView();
		virtual ~MainView();

		virtual void Draw(sf::RenderWindow& window) const override;

	private:
		virtual sf::FloatRect GetLogRect() const override;
		virtual sf::FloatRect GetClipRect() const override;
		virtual void OnMouseMoved(const sf::Vector2i& point) override;
		virtual void OnMouseDown(sf::Mouse::Button button, const sf::Vector2i& point) override;

		Grid m_grid;
		std::vector<std::unique_ptr<Model::Object>> m_objects;
		std::unique_ptr<Tools::Tool> m_tool;
	};
}

