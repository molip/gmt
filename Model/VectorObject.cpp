#include "VectorObject.h"
#include "../Grid.h"
#include "../WallMaker.h"

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
	const float FloorTextureScale = 0.1f;
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
	rc.GetWindow().draw(m_innerWalls, renderStates);
	rc.GetWindow().draw(m_pillars, renderStates);
	rc.GetWindow().draw(m_outerWalls, renderStates);
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

	m_floors = GetFloors();

	WallMaker wallMaker(*m_edgeMesh, m_wallTexture->getSize());
	m_innerWalls = wallMaker.GetInnerWalls();
	m_outerWalls = wallMaker.GetOuterWalls();
	m_pillars = wallMaker.GetPillars();
}

sf::VertexArray VectorObject::GetFloors() const
{
	sf::VertexArray floors(sf::Triangles);

	const Jig::Vec2f texSize(m_floorTexture->getSize());

	if (texSize.IsZero())
		return {};

	for (auto& face : m_edgeMesh->GetFaces())
	{
		auto poly = face->GetPolygon();
		poly.Update();
		if (!poly.IsSelfIntersecting())
		{
			const auto mesh = MakeTriangleMesh(Jig::Triangulator(poly).Go());
			for (auto& point : *mesh)
				floors.append(sf::Vertex(point, { point.x * texSize.x * FloorTextureScale, point.y * texSize.y * FloorTextureScale }));
		}
	}

	return floors;
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
