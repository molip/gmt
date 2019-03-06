#pragma once

#include "Tool.h"

namespace GMT::Model::Command
{
	class EdgeMesh;
}

namespace GMT::Tools
{
	class AddWallThingTool : public Tool
	{
	public:
		AddWallThingTool(const MainView& view);

		virtual void Draw(RenderContext& rc) const override;
		virtual void OnMouseMoved(const sf::Vector2i& point) override;
		virtual void OnMouseDown(sf::Mouse::Button button, const sf::Vector2i& point) override;
		virtual void OnMouseUp(sf::Mouse::Button button, const sf::Vector2i& point) override;
		virtual void OnKeyPressed(const sf::Event::KeyEvent event) override;
		virtual void Update() override { m_overState = nullptr; }

	private:
		void Update(const sf::Vector2f& logPoint);

		Model::ElementPtr m_overState;
	};
}
