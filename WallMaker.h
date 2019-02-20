#pragma once

#include "Jig/EdgeMesh.h"

#include <SFML/Graphics.hpp>

#include <set>

namespace Jig
{
	class EdgeMesh;
}

namespace GMT
{
	class WallMaker
	{
	public:
		WallMaker(const Jig::EdgeMesh& mesh, const Jig::Vec2f& texSize, const Jig::Vec2f& pillarTexSize);

		sf::VertexArray&& GetInnerWalls() { return std::move(m_innerWalls); }
		sf::VertexArray&& GetOuterWalls() { return std::move(m_outerWalls); }
		sf::VertexArray&& GetPillars() { return std::move(m_pillars); }

	private:
		void AddOuterWalls();
		void AddInnerWalls();
		void AddWall(const Jig::Vec2f& outer0, const Jig::Vec2f& outer1, const Jig::Vec2f& inner1, const Jig::Vec2f& inner0, float thickness, sf::VertexArray& array);
		void AddPillar(const Jig::Vec2f& point);

		const Jig::EdgeMesh& m_mesh;
		const Jig::Vec2f& m_texSize, m_pillarTexSize;
		sf::VertexArray m_innerWalls, m_outerWalls, m_pillars;
		std::set<const Jig::EdgeMesh::Vert*> m_pillarVerts;
	};
}
