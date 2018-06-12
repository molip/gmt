#pragma once

#include "Tool.h"

#include "Jig/EdgeMesh.h"

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
		enum class Snap { None, Grid, Object };

		struct OverState
		{
			const Model::VectorObject* object{};
			const Jig::EdgeMesh::Face* room{};
			Jig::EdgeMesh::VertPtr vertex{};
			sf::Vector2f gridPoint; // Model coords.
			Snap snap = Snap::None;
		};

		struct ObjectEdit
		{
			const Model::VectorObject* object{};
			const Jig::EdgeMesh::Face* room{};
			Jig::EdgeMesh::VertPtr start{}, end{};
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
