#pragma once

#include "Object.h"

#include "Jig/EdgeMesh.h"
#include "Jig/VectorFwd.h"

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

#include <optional>
#include <vector>

namespace Jig
{
	class PolyLine;
	enum class LineAlignment;
}

namespace sf
{
	class Texture;
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

		void UpdateFloors();
		void UpdateWalls();
		void UpdateWalls(const Jig::PolyLine& polyline, Jig::LineAlignment alignment);

		TriangleMeshPtr MakeTriangleMesh(const Jig::EdgeMesh& edgeMesh) const;

		sf::VertexArray	m_floors, m_walls;

		std::unique_ptr<Jig::EdgeMesh> m_edgeMesh;
		std::unique_ptr<sf::Texture> m_wallTexture, m_floorTexture;
	};
}
