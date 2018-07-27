#include "VectorObject.h"
#include "../Grid.h"

#include "Jig/EdgeMeshInternalEdges.h"
#include "Jig/Geometry.h"
#include "Jig/Mitre.h"
#include "Jig/Polygon.h"
#include "Jig/Triangulator.h"

#include "libKernel/FormatString.h"
#include "libKernel/Log.h"
#include "libKernel/Math.h"
#include "libKernel/MinFinder.h"

#include <SFML/Graphics.hpp>

using namespace GMT;
using namespace GMT::Model;

namespace
{
	const float WallThickness = 1.0f;
	const float FloorTextureScale = 0.1f;
	const Jig::Vec2f WallTextureScale(0.5f, 0.5f);
}

REGISTER_DYNAMIC(VectorObject)

std::unique_ptr<VectorObject> VectorObject::Create(std::vector<sf::Vector2f> points)
{
	if (points.empty())
		return nullptr;

	// Polygon::Update() doesn't like duplicate adjacent points.
	points.erase(std::unique(points.begin(), points.end()), points.end());

	if (points.size() < 3)
		return nullptr;

	Jig::Polygon poly;
	poly.reserve(points.size());

	for (const auto& point : points)
		poly.push_back({ (double)point.x, (double)point.y });

	poly.Update();
	if (poly.IsSelfIntersecting())
		return nullptr;

	return std::unique_ptr<VectorObject>(new VectorObject(poly));
}

VectorObject::VectorObject()
{
	m_wallTexture = std::make_unique<sf::Texture>();
	m_wallTexture->loadFromFile("wall.jpg");
	m_wallTexture->setRepeated(true);

	m_floorTexture = std::make_unique<sf::Texture>();
	m_floorTexture->loadFromFile("floor.jpg");
	m_floorTexture->setRepeated(true);

	m_floors.setPrimitiveType(sf::Triangles);
	m_walls.setPrimitiveType(sf::Triangles);
}

VectorObject::VectorObject(const Jig::Polygon& poly) : VectorObject()
{
	std::vector<Jig::EdgeMesh::VertPtr> verts;
	verts.reserve(poly.size());
	for (const auto& point : poly)
		verts.push_back(std::make_unique<Jig::EdgeMesh::Vert>(point));

	m_edgeMesh = std::make_unique<Jig::EdgeMesh>(std::move(verts));

	auto face = std::make_unique<Jig::EdgeMesh::Face>();
	for (const auto& vert : m_edgeMesh->GetVerts())
		face->AddAndConnectEdge(vert.get());

	m_edgeMesh->PushFace(std::move(face));

	Update();
}

VectorObject::~VectorObject() = default;

void VectorObject::Save(Kernel::Serial::SaveNode & node) const
{
	node.SaveClassPtr("mesh", m_edgeMesh);
}

void VectorObject::Load(const Kernel::Serial::LoadNode & node)
{
	node.LoadClassPtr("mesh", m_edgeMesh);
	Update();
}

void VectorObject::Draw(RenderContext& rc) const
{
	sf::RenderStates renderStates(rc.GetGrid().GetTransform());

	renderStates.texture = m_floorTexture.get();
	rc.GetWindow().draw(m_floors, renderStates);

	renderStates.texture = m_wallTexture.get();
	rc.GetWindow().draw(m_walls, renderStates);
}

VectorObject::TriangleMeshPtr VectorObject::MakeTriangleMesh(const Jig::EdgeMesh& edgeMesh) const
{
	TriangleMeshPtr mesh = std::make_unique<TriangleMesh>();
	mesh->reserve(edgeMesh.GetFaces().size() * 3);
	for (auto& face : edgeMesh.GetFaces())
		for (auto& point : face->GetPointLoop())
			mesh->push_back({ (float)point.x, (float)point.y });

	return mesh;
}

void VectorObject::Update() const
{
	m_edgeMesh->Update();
	m_edgeMesh->Dump();

	UpdateFloors();
	UpdateWalls();
}

void VectorObject::UpdateFloors() const
{
	m_floors.clear();

	const Jig::Vec2f texSize(m_floorTexture->getSize());

	if (texSize.IsZero())
		return;

	for (auto& face : m_edgeMesh->GetFaces())
	{
		auto poly = face->GetPolygon();
		poly.Update();
		if (!poly.IsSelfIntersecting())
		{
			const auto mesh = MakeTriangleMesh(Jig::Triangulator(poly).Go());
			for (auto& point : *mesh)
				m_floors.append(sf::Vertex(point, { point.x * texSize.x * FloorTextureScale, point.y * texSize.y * FloorTextureScale }));
		}
	}
}

void VectorObject::UpdateWalls() const
{
	m_walls.clear();

	UpdateWalls(m_edgeMesh->GetOuterPolygon(), Jig::LineAlignment::Outer);

	Jig::EdgeMeshInternalEdges emie(*m_edgeMesh);

	for (auto& line : emie.m_lines)
	{
		Jig::PolyLine poly{ line.GetP0(), line.GetP1() };
		UpdateWalls(poly, Jig::LineAlignment::Centre);
	}
}

void VectorObject::UpdateWalls(const Jig::PolyLine& polyline, Jig::LineAlignment alignment) const
{
	Jig::PolyLine inner, outer;
	inner.SetClosed(polyline.IsClosed());
	outer.SetClosed(polyline.IsClosed());

	for (int i = 0; i < polyline.size(); ++i)
	{
		Jig::Vec2f prev, next;
		const Jig::Vec2f point = polyline.GetVertex(i);
		
		const Jig::Vec2f *pPrev{}, *pNext{};

		if (polyline.IsValidIndex(i - 1))
		{
			prev = polyline.GetVertex(i - 1);
			pPrev = &prev;
		}

		if (polyline.IsValidIndex(i + 1))
		{
			next = polyline.GetVertex(i + 1);
			pNext = &next;
		}

		auto wallPoints = Jig::GetMitrePoints(point, pPrev, pNext, ::WallThickness, alignment);
		inner.push_back((*wallPoints).first);
		outer.push_back((*wallPoints).second);
	}

	inner.Update();
	outer.Update();

	if (inner.IsSelfIntersecting() || outer.IsSelfIntersecting())
		return;

	for (int i = 0; i < outer.GetSegmentCount(); ++i)
	{
		const Jig::Vec2f outer0 = outer.GetVertex(i);
		const Jig::Vec2f inner0 = inner.GetVertex(i);
		const Jig::Vec2f outer1 = outer.GetVertex(i + 1);
		const Jig::Vec2f inner1 = inner.GetVertex(i + 1);
	
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

		const Jig::Vec2f texSize(m_wallTexture->getSize());

		auto addPoint = [&](auto& point, auto& tex)
		{
			m_walls.append(sf::Vertex(point, { tex.x * texSize.x * WallTextureScale.x, tex.y * texSize.y * WallTextureScale.y }));
		};

		addPoint(outer0, outerTex0);
		addPoint(outer1, outerTex1);
		addPoint(inner1, innerTex1);
		addPoint(inner1, innerTex1);
		addPoint(inner0, innerTex0);
		addPoint(outer0, outerTex0);
	}
}

using HitTester = Kernel::MinFinder<VectorObject::Terminus, float>;

namespace
{
	template <typename T>
	void HitTestEdges(const sf::Vector2f& point, float tolerance, const T& loop, HitTester& hitTester)
	{
		for (const Jig::EdgeMesh::Edge& edge : loop)
		{
			const auto line = Jig::Line2::MakeFinite(*edge.vert, *T::GetNext(edge).vert);
			Jig::Rect bbox = line.GetBBox();
			bbox.Inflate(tolerance, tolerance);
			if (!bbox.Contains(point))
				continue;
			
			Jig::Vec2 intersect;
			if (line.PerpIntersect(point, nullptr, &intersect))
			{
				if (!line.IsVertical())
				{
					double x = int(intersect.x + 0.5);
					if (x > bbox.m_p0.x && x < bbox.m_p1.x && !Kernel::fcomp(x, line.GetP0().x) && !Kernel::fcomp(x, line.GetP1().x))
					{
						const Jig::Vec2 snapped(x, intersect.y + (x - intersect.x) * line.GetGradient());
						hitTester.Try(VectorObject::EdgeTerminus(&edge, snapped), Jig::Vec2f(Jig::Vec2f(snapped) - point).GetLengthSquared());
					}
				}

				if (!line.IsHorizontal())
				{
					double y = int(intersect.y + 0.5);
					if (y > bbox.m_p0.y && y < bbox.m_p1.y && !Kernel::fcomp(y, line.GetP0().y) && !Kernel::fcomp(y, line.GetP1().y))
					{
						double dx = line.IsVertical() ? 0 : (y - intersect.y) / line.GetGradient();
						const Jig::Vec2 snapped(intersect.x + dx, y);
						hitTester.Try(VectorObject::EdgeTerminus(&edge, snapped), Jig::Vec2f(Jig::Vec2f(snapped) - point).GetLengthSquared());
					}
				}
			}
		}
	}
}

VectorObject::Terminus VectorObject::HitTestEdges(const sf::Vector2f& point, float tolerance, float& distSquared) const
{
	HitTester hitTester(tolerance);
	
	if (auto* vert = m_edgeMesh->FindNearestVert(point, tolerance))
		hitTester.Try(vert, (float)(Jig::Vec2(Jig::Vec2(point) - *vert).GetLengthSquared()));

	if (auto* face = HitTestRooms(point))
		::HitTestEdges(point, tolerance, face->GetEdges(), hitTester);
	else
	{
		const Jig::EdgeMesh& edgeMesh = *m_edgeMesh;
		::HitTestEdges(point, tolerance, edgeMesh.GetOuterEdges(), hitTester);
	}

	distSquared = hitTester.GetValue();
	return hitTester.GetObject();
}

const Jig::EdgeMesh::Face* VectorObject::HitTestRooms(const sf::Vector2f& point) const
{
	return m_edgeMesh->HitTest(Jig::Vec2(point));
}

void VectorObject::Terminus::Log(int indent) const
{
	if (auto vert = GetVert())
		Kernel::Log(indent) << "Vertex: pos=" << *vert << std::endl;
	else if (auto edge = GetEdge())
		Kernel::Log(indent) << "Edge: vertex pos=" << edge->second << std::endl;
	else
		Kernel::Log(indent) << "Null" << std::endl;
}
