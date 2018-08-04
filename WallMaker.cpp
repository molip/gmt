#include "WallMaker.h"

#include "Jig/Mitre.h"
#include "Jig/Polygon.h"

using namespace GMT;

namespace
{
	const float WallThickness = 1.0f;
	const Jig::Vec2f WallTextureScale(0.33f, 0.33f);
}

sf::VertexArray&& WallMaker::GetWalls()
{
	m_walls.setPrimitiveType(sf::Triangles);
	AddOuterWalls();
	AddInnerWalls();
	return std::move(m_walls);
}

void WallMaker::AddOuterWalls()
{
	Jig::Polygon poly = m_mesh.GetOuterPolygon();

	for (int i = 0; i < poly.size(); ++i)
	{
		auto get = [&](int i) { return Jig::GetMitrePoints(poly.GetVertex(i), &poly.GetVertex(i - 1), &poly.GetVertex(i + 1), ::WallThickness, Jig::LineAlignment::Outer); };
		auto these = get(i);
		auto next = get(i + 1);

		if (these.has_value() && next.has_value())
			AddWall(these->first, next->first, next->second, these->second);
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
					Jig::Vec2 cross;
					if (!edge.prev->twin || !edge.next->twin)
						cross = Jig::GetMitreVec(*edge.vert, (Jig::Vec2*)nullptr, edge.next->vert, ::WallThickness / 2).value();

					if (edge.prev->twin)
					{
						these = getMitrePoints(edge, ::WallThickness / 2, Jig::LineAlignment::Inner).value();
					}
					else
					{
						auto line = Jig::Line2::MakeInfinite(*edge.vert - cross, *edge.next->vert - cross);
						Jig::Vec2 point;
						if (!line.Intersect(Jig::Line2::MakeInfinite(*edge.vert, *edge.prev->vert), &point))
							continue;

						these.first = point;
						these.second = *edge.vert;
					}

					if (edge.next->twin)
					{
						next = getMitrePoints(*edge.next, ::WallThickness / 2, Jig::LineAlignment::Inner).value();
					}
					else
					{
						auto line = Jig::Line2::MakeInfinite(*edge.vert - cross, *edge.next->vert - cross);
						Jig::Vec2 point;
						if (!line.Intersect(Jig::Line2::MakeInfinite(*edge.next->vert, *edge.next->next->vert), &point))
							continue;

						next.first = point;
						next.second = *edge.next->vert;
					}

					AddWall(these.first, next.first, next.second, these.second);
				}
			}
			catch (std::bad_optional_access)
			{
			}
		}
	}
}

void WallMaker::AddWall(const Jig::Vec2f& outer0, const Jig::Vec2f& outer1, const Jig::Vec2f& inner1, const Jig::Vec2f& inner0)
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
		m_walls.append(sf::Vertex(point, { tex.x * m_texSize.x * WallTextureScale.x, tex.y * m_texSize.y * WallTextureScale.y }));
	};

	addPoint(outer0, outerTex0);
	addPoint(outer1, outerTex1);
	addPoint(inner1, innerTex1);
	addPoint(inner1, innerTex1);
	addPoint(inner0, innerTex0);
	addPoint(outer0, outerTex0);
}

