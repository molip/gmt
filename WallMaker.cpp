#include "WallMaker.h"

#include "Jig/Mitre.h"
#include "Jig/Polygon.h"

#include <math.h>

using namespace GMT;

namespace
{
	const float WallThickness = 1.0f;
	const Jig::Vec2f WallTextureScale(0.33f, 0.33f);
	const double MinSlideThreshold = M_PI * 0.9;
}

sf::VertexArray&& WallMaker::GetWalls()
{
	m_walls.setPrimitiveType(sf::Triangles);
	AddOuterWalls();
	CalculateSlides();
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

void WallMaker::CalculateSlides()
{
	using AlignMap = std::map<const Jig::EdgeMesh::Vert*, const Jig::EdgeMesh::Edge*>;
	AlignMap aligns;

	for (auto& e : m_mesh.GetOuterEdges())
	{
		if (!e.prev->twin) // No internal edges on this vert.
			continue;

		try
		{
			const auto& prevOuter = *e.FindPrevOuterEdge();

			const Jig::Vec2 prevOuterNorm = prevOuter.GetVec().Normalised();
			const Jig::Vec2 edgeNorm = e.GetVec().Normalised();

			const double outerAngle = M_PI + prevOuterNorm.GetAngle(edgeNorm);

			// Get angles of first/last internal edges. 
			const double a1 = M_PI - prevOuterNorm.GetAngle(prevOuter.next->GetVec().Normalised());
			const double a2 = M_PI - e.prev->GetVec().Normalised().GetAngle(edgeNorm);

			double threshold = std::max(MinSlideThreshold, M_PI - outerAngle);

			Jig::Vec2 slide;
			if (a1 > threshold || a2 > threshold) 
				aligns[e.vert] = a1 > a2 ? prevOuter.next : e.prev; // Internal edge we will align to vertex.
		}
		catch (Jig::DegenerateException)
		{
			continue;
		}
	}

	//TODO: Propagate aligns to internal verts.

	auto FindAlignEdge = [&aligns](const Jig::EdgeMesh::Vert* vert)
	{
		auto it = aligns.find(vert);
		return it == aligns.end() ? nullptr : it->second;
	};

	for (auto&[vert, edge] : aligns)
	{
		const bool slideForward = edge->vert == vert;

		const auto alignEdgeVec = edge->GetVec();
		const auto* otherVert = slideForward ? edge->next->vert : edge->vert;
		
		try
		{
			Jig::Vec2 slideVecNorm = slideForward ? edge->prev->FindNextOuterEdge()->GetVec().Normalised() : -edge->next->FindPrevOuterEdge()->GetVec().Normalised();
			Jig::Vec2 alignEdgeVecNorm = alignEdgeVec.Normalised();

			const double vertAngle = M_PI - (slideForward ? Jig::Vec2(-alignEdgeVecNorm).GetAngle(slideVecNorm) : Jig::Vec2(-slideVecNorm).GetAngle(-alignEdgeVecNorm));

			const auto* otherAlignEdge = FindAlignEdge(otherVert);

			double pivotWidth = 0;
			if (!otherAlignEdge)
				pivotWidth = WallThickness / 2;
			else if (otherAlignEdge != edge)
				pivotWidth = WallThickness;

			const double pivotAngle = asin(pivotWidth / alignEdgeVec.GetLength());
			const double slideLength = WallThickness / 2 / sin(vertAngle + pivotAngle);
			m_points[vert] = *vert + slideVecNorm * slideLength;
		}
		catch (Jig::DegenerateException)
		{
			continue;
		}
	}
}

void WallMaker::AddInnerWalls()
{
	auto getMitrePoints = [&](const Jig::EdgeMesh::Edge& edge, float thickness, Jig::LineAlignment align)
	{
		return Jig::GetMitrePoints(GetAdjusted(edge.vert), &GetAdjusted(edge.prev->vert), &GetAdjusted(edge.next->vert), thickness, align);
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
					Jig::Line2 line;
					if (!edge.prev->twin || !edge.next->twin)
					{
						auto cross = Jig::GetMitreVec(GetAdjusted(edge.vert), (Jig::Vec2*)nullptr, &GetAdjusted(edge.next->vert), ::WallThickness / 2).value();
						line = Jig::Line2::MakeInfinite(GetAdjusted(edge.vert) - cross, GetAdjusted(edge.next->vert) - cross);
					}

					if (edge.prev->twin)
					{
						these = getMitrePoints(edge, ::WallThickness / 2, Jig::LineAlignment::Inner).value();
					}
					else
					{
						Jig::Vec2 point;
						if (!line.Intersect(Jig::Line2::MakeFinite(*edge.vert, *edge.prev->vert), &point))
							point = *edge.vert;

						these.first = point;
						these.second = GetAdjusted(edge.vert);
					}

					if (edge.next->twin)
					{
						next = getMitrePoints(*edge.next, ::WallThickness / 2, Jig::LineAlignment::Inner).value();
					}
					else
					{
						Jig::Vec2 point;
						if (!line.Intersect(Jig::Line2::MakeFinite(*edge.next->vert, *edge.next->next->vert), &point))
							point = *edge.next->vert;

						next.first = point;
						next.second = GetAdjusted(edge.next->vert);
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

const Jig::Vec2& WallMaker::GetAdjusted(const Jig::EdgeMesh::Vert* vert) const
{
	auto it = m_points.find(vert);
	return it == m_points.end() ? *vert : it->second;
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

