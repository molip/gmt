#pragma once

#include "Tool.h"

namespace GMT::Tools
{
	class AddVertTool : public Tool
	{
	public:
		AddVertTool(const MainView& view);

		virtual void Draw(RenderContext& rc) const override;
		virtual void OnMouseMoved(const sf::Vector2i& point) override;
		virtual void OnMouseDown(sf::Mouse::Button button, const sf::Vector2i& point) override;

	private:
		void Update(const sf::Vector2f& logPoint);

		Model::EdgePointElementPtr m_overState;
	};
}
