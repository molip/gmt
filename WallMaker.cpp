#include "WallMaker.h"

#include "Jig/Mitre.h"
#include "Jig/Polygon.h"

using namespace GMT;

namespace
{
	const float OuterWallThickness = 0.5f;
	const float InnerWallThickness = 0.3f;
	const float PillarSize = 0.5f;
	const Jig::Vec2f WallTextureScale(1.0f, 1.0f);
}

WallMaker::WallMaker(const Jig::EdgeMesh& mesh, const Jig::Vec2f& texSize, const Jig::Vec2f& pillarTexSize) : m_mesh(mesh), m_texSize(texSize), m_pillarTexSize(pillarTexSize)
{
	m_innerWalls.setPrimitiveType(sf::Triangles);
	m_outerWalls.setPrimitiveType(sf::Triangles);
	m_pillars.setPrimitiveType(sf::Quads);
	
	AddOuterWalls();
	AddInnerWalls();

	for (auto* vert : m_pillarVerts)
		AddPillar(*vert);
}

void WallMaker::AddOuterWalls()
{
	Jig::Polygon poly = m_mesh.GetOuterPolygon();

	for (int i = 0; i < poly.size(); ++i)
	{
		auto get = [&](int i) { return Jig::GetMitrePoints(poly.GetVertex(i), &poly.GetVertex(i - 1), &poly.GetVertex(i + 1), ::OuterWallThickness, Jig::LineAlignment::Outer); };
		auto these = get(i);
		auto next = get(i + 1);

		if (these.has_value() && next.has_value())
			AddWall(these->first, next->first, next->second, these->second, ::OuterWallThickness, m_outerWalls);
	}
}

void WallMaker::AddInnerWalls()
{
	auto getMitrePoints = [](const Jig::EdgeMesh::Edge& edge, float thickness, Jig::LineAlignment align)
	{
		return Jig::GetMitrePoints(*edge.vert, edge.prev->vert, edge.next->vert, thickness, align);
	};

	for (auto& face : m_mesh.GetFaces())
	{
		for (auto& edge : face->GetEdges())
		{
			try
			{
				std::pair<Jig::Vec2, Jig::Vec2> these, next;
				if (edge.twin)
				{
					const float thickness = ::InnerWallThickness / 2;

					Jig::Vec2 cross;
					if (!edge.prev->twin || !edge.next->twin)
						cross = Jig::GetMitreVec(*edge.vert, (Jig::Vec2*)nullptr, edge.next->vert, thickness).value();

					if (edge.prev->twin) // Internal start vert.
					{
						these = getMitrePoints(edge, thickness, Jig::LineAlignment::Inner).value();
					}
					else
					{
						these.first = *edge.vert - cross;
						these.second = *edge.vert;

						m_pillarVerts.insert(edge.vert);
					}
					
					if (edge.next->twin) // Internal end vert.
					{
						next = getMitrePoints(*edge.next, thickness, Jig::LineAlignment::Inner).value();
					}
					else
					{
						next.first = *edge.next->vert - cross;
						next.second = *edge.next->vert;
					}

					AddWall(these.first, next.first, next.second, these.second, thickness, m_innerWalls);
				}
			}
			catch (std::bad_optional_access)
			{
			}
		}
	}
}

void WallMaker::AddWall(const Jig::Vec2f& outer0, const Jig::Vec2f& outer1, const Jig::Vec2f& inner1, const Jig::Vec2f& inner0, float thickness, sf::VertexArray& array)
{
	const Jig::Vec2f outerVec(outer1 - outer0);
	const Jig::Vec2f innerVec(inner1 - inner0);
	const Jig::Vec2f edgeVec(inner0 - outer0);

	const float outerLength = outerVec.GetLength();
	const float innerLength = innerVec.GetLength();
	const float edgeLength = edgeVec.GetLength();

	Jig::Vec2f outerVecNorm = outerVec;
	Jig::Vec2f edgeVecNorm = edgeVec;
	if (!outerVecNorm.Normalise() || !edgeVecNorm.Normalise())
		return;

	const float cos = outerVecNorm.Dot(edgeVecNorm);

	const float innerOffset = cos * edgeLength;

	const Jig::Vec2f outerTex0;
	const Jig::Vec2f outerTex1(outerLength, 0);
	const Jig::Vec2f innerTex0(innerOffset, 1);
	const Jig::Vec2f innerTex1(innerOffset + innerLength, 1);

	auto addPoint = [&](auto& point, auto& tex)
	{
		array.append(sf::Vertex(point, { tex.x * m_texSize.x * WallTextureScale.x, tex.y * m_texSize.y * WallTextureScale.y * thickness }));
	};

	addPoint(outer0, outerTex0);
	addPoint(outer1, outerTex1);
	addPoint(inner1, innerTex1);
	addPoint(inner1, innerTex1);
	addPoint(inner0, innerTex0);
	addPoint(outer0, outerTex0);
}

void WallMaker::AddPillar(const Jig::Vec2f& point)
{
	Jig::Vec2f topLeft = point - Jig::Vec2f(::PillarSize, ::PillarSize) * 0.5f;

	auto addPoint = [&](float x, float y)
	{
		m_pillars.append(sf::Vertex(topLeft + Jig::Vec2f(x * ::PillarSize, y * ::PillarSize), { x * m_pillarTexSize.x, y * m_pillarTexSize.y }));
	};

	addPoint(0, 0);
	addPoint(1, 0);
	addPoint(1, 1);
	addPoint(0, 1);
}
