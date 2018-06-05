#pragma once

#include "Object.h"

#include "Jig/VectorFwd.h"

#include <SFML/System.hpp>

#include <optional>
#include <vector>

namespace Jig
{
	class EdgeMesh;
	class Polygon;
}

namespace GMT::Model
{
	class VectorObject : public Object
	{
	public:
		VectorObject();
		virtual ~VectorObject();

		virtual void Draw(RenderContext& rc) const override;

		bool IsClosed() const;
		std::vector<sf::Vector2i>& GetPoints() { return m_points; }

		bool Tesselate();

	private:
		using WallPoints = std::optional<std::pair<Jig::Vec2f, Jig::Vec2f>>;
		using Mesh = std::vector<sf::Vector2f>;
		using MeshPtr = std::unique_ptr<Mesh>;

		MeshPtr TesselateWall(const Jig::Polygon& poly) const;
		VectorObject::MeshPtr MakeMesh(const Jig::EdgeMesh& edgeMesh) const;

		std::vector<sf::Vector2i> m_points;
		MeshPtr m_floor, m_walls;
	};
}
