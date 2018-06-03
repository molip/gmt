#pragma once

#include "Tool.h"

namespace GMT
{
	class MainView;
}

namespace GMT::Model
{
	class VectorObject;
}

namespace GMT::Tools
{
	class VectorTool : public Tool
	{
	public:
		VectorTool(const MainView& view);

		virtual void Draw(RenderContext& rc) const override;
		virtual void OnMouseMoved(const sf::Vector2i& point) override;
		virtual void OnMouseDown(sf::Mouse::Button button, const sf::Vector2i& point) override;

	private:
		const MainView& m_view;
		sf::Vector2i m_gridPoint;
		bool m_snapped = false;
		std::unique_ptr<Model::VectorObject> m_object;
		std::vector<std::unique_ptr<Model::VectorObject>> m_objects;
	};
}
