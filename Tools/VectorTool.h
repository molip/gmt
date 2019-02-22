#pragma once

#include "Tool.h"

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
		struct ObjectEdit
		{
			const Model::VectorObject* object{};
			const Jig::EdgeMesh::Face* room{};
			Model::ElementPtr start;
		};

		bool IsClosed() const;
		void Update(const sf::Vector2f& logPoint);
		bool AddWall(const Jig::PolyLine& polyline, Model::ElementPtr start, Model::ElementPtr end);

		std::vector<sf::Vector2f> m_points;
		Model::ElementPtr m_overState;
		
		std::unique_ptr<ObjectEdit> m_objectEdit;
	};
}
