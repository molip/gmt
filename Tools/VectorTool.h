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
	class VectorTool : public Tool
	{
	public:
		VectorTool(const MainView& view);

		virtual void Draw(RenderContext& rc) const override;
		virtual void OnMouseMoved(const sf::Vector2i& point) override;
		virtual void OnMouseDown(sf::Mouse::Button button, const sf::Vector2i& point) override;
		virtual void OnKeyPressed(const sf::Event::KeyEvent event) override;

	private:
		enum class Snap { None, Grid, Vert, Edge};
		using Terminus = Model::VectorObject::Terminus;

		struct OverState
		{
			const Model::VectorObject* object{};
			const Jig::EdgeMesh::Face* room{};
			Terminus terminus{};
			sf::Vector2f gridPoint; // Model coords.
			Snap snap = Snap::None;
		};

		struct ObjectEdit
		{
			const Model::VectorObject* object{};
			const Jig::EdgeMesh::Face* room{};
			Terminus start, end;
		};

		bool IsClosed() const;
		void Update(const sf::Vector2f& logPoint);
		OverState HitTest(const sf::Vector2f& logPoint, const Model::VectorObject* special) const;

		const MainView& m_view;
		std::vector<sf::Vector2f> m_points;
		OverState m_overState;
		
		std::unique_ptr<ObjectEdit> m_objectEdit;
	};
}
