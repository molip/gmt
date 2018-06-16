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

	private:
		using WallPoints = std::optional<std::pair<Jig::Vec2f, Jig::Vec2f>>;
		using TriangleMesh = std::vector<sf::Vector2f>;
		using TriangleMeshPtr = std::unique_ptr<TriangleMesh>;

		bool Tesselate(std::vector<sf::Vector2f> points);
		TriangleMeshPtr TesselateWall(const Jig::Polygon& poly) const;
		TriangleMeshPtr MakeTriangleMesh(const Jig::EdgeMesh& edgeMesh) const;

		TriangleMeshPtr m_floor, m_walls;

		std::unique_ptr<Jig::EdgeMesh> m_edgeMesh;
	};
}
