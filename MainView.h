#pragma once

#include "View.h"
#include "Grid.h"

#include <memory>

namespace GMT
{
	class Object;

	class MainView : public View
	{
	public:
		MainView();
		virtual ~MainView();

		virtual void Draw(sf::RenderWindow& window) const override;

	private:
		virtual sf::FloatRect GetLogRect() const override;
		virtual sf::FloatRect GetClipRect() const override;

		std::unique_ptr<Object> m_object;
		Grid m_grid;
	};
}

