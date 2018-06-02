#pragma once

#include "View.h"
#include "Grid.h"

#include <memory>

namespace GMT
{
	namespace Model
	{
		class Object;
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

		Grid m_grid;
		std::unique_ptr<Model::Object> m_object;
	};
}

