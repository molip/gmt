#pragma once

#include "Tool.h"

#include "../Model/VectorObject.h"

#include "Jig/EdgeMesh.h"

namespace GMT
{
	class MainView;
}

namespace GMT::Model::Command
{
	class EdgeMesh;
}

namespace GMT::Tools
{
	class SelectTool : public Tool
	{
	public:
		SelectTool(const MainView& view);

		virtual void Draw(RenderContext& rc) const override;
		virtual void OnMouseMoved(const sf::Vector2i& point) override;
		virtual void OnMouseDown(sf::Mouse::Button button, const sf::Vector2i& point) override;
		virtual void OnMouseUp(sf::Mouse::Button button, const sf::Vector2i& point) override;
		virtual void OnKeyPressed(const sf::Event::KeyEvent event) override;

	private:
		enum class Snap { None, Grid, Vert, Edge};
		using Terminus = Model::VectorObject::Terminus;

		struct OverState
		{
			const Jig::EdgeMesh::Vert* vert{};
			const Model::VectorObject* object{};
		};

		void Update(const sf::Vector2f& logPoint);

		const MainView& m_view;
		OverState m_overState;
		Jig::Vec2f m_gridPoint;

		bool m_dragging{};
		std::unique_ptr<Model::Command::EdgeMesh> m_command;
	};
}
