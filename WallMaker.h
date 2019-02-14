#pragma once

#include "Jig/EdgeMesh.h"

#include <SFML/Graphics.hpp>

namespace GMT
{
	class WallMaker
	{
	public:
		WallMaker(const Jig::EdgeMesh& mesh, const Jig::Vec2f& texSize) : m_mesh(mesh), m_texSize(texSize) {}

		sf::VertexArray&& GetWalls();

	private:
		void AddOuterWalls();
		void AddInnerWalls();
		void AddWall(const Jig::Vec2f& outer0, const Jig::Vec2f& outer1, const Jig::Vec2f& inner1, const Jig::Vec2f& inner0);
		void CalculateSlides();

		const Jig::Vec2& GetAdjusted(const Jig::EdgeMesh::Vert* vert) const;
			
		const Jig::EdgeMesh& m_mesh;
		const Jig::Vec2f& m_texSize;
		sf::VertexArray m_walls;
		std::map<const Jig::EdgeMesh::Vert*, Jig::Vec2> m_points;
	};
}
