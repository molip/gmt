#pragma once

#include "Tool.h"

#include "../Model/VectorObject.h"

#include "Jig/EdgeMesh.h"

namespace GMT
{
	class MainView;
}

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
		using Terminus = Model::VectorObject::EdgeTerminus;

		struct OverState
		{
			const Model::VectorObject* object{};
			const Jig::EdgeMesh::Face* room{};
			Terminus terminus{};
		};

		void Update(const sf::Vector2f& logPoint);
		OverState HitTest(const sf::Vector2f& logPoint) const;

		const MainView& m_view;
		OverState m_overState;
	};
}
