#pragma once

#include "Object.h"

#include "Jig/EdgeMesh.h"
#include "Jig/VectorFwd.h"

#include <SFML/System.hpp>

#include <optional>
#include <vector>

namespace Jig
{
	class Polygon;
}

namespace GMT::Model
{
	class VectorObject : public Object
	{
	public:
		VectorObject(const std::vector<sf::Vector2f>& points);
		virtual ~VectorObject();

		virtual void Draw(RenderContext& rc) const override;

		const Jig::EdgeMesh::Vert* FindNearestVert(const sf::Vector2f& point, float tolerance) const;
		const Jig::EdgeMesh::Face* HitTestRooms(const sf::Vector2f& point) const;

		bool AddWall(const Jig::PolyLine& polyline, const Jig::EdgeMesh::Vert& start, const Jig::EdgeMesh::Vert& end);

	private:
		using WallPoints = std::optional<std::pair<Jig::Vec2f, Jig::Vec2f>>;
		using TriangleMesh = std::vector<sf::Vector2f>;
		using TriangleMeshPtr = std::unique_ptr<TriangleMesh>;

		bool Init(std::vector<sf::Vector2f> points);
		void Update();

		TriangleMeshPtr TesselateWall(const Jig::Polygon& poly) const;
		TriangleMeshPtr MakeTriangleMesh(const Jig::EdgeMesh& edgeMesh) const;

		std::vector<TriangleMeshPtr> m_floors, m_walls;

		std::unique_ptr<Jig::EdgeMesh> m_edgeMesh;
	};
}
