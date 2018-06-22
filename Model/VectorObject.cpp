#include "VectorObject.h"
#include "../Grid.h"

#include "Jig/EdgeMeshAddFace.h"
#include "Jig/Geometry.h"
#include "Jig/Line2.h"
#include "Jig/Mitre.h"
#include "Jig/Polygon.h"
#include "Jig/Triangulator.h"

#include <SFML/Graphics.hpp>

using namespace GMT;
using namespace GMT::Model;

namespace
{
	float WallThickness = 0.5f; // Logical.
}

VectorObject::VectorObject(const std::vector<sf::Vector2f>& points)
{
	Init(points);
}

VectorObject::~VectorObject() = default;

void VectorObject::Draw(RenderContext& rc) const
{
	auto addPoint = [&](auto& verts, auto& point, auto colour)
	{
		verts.append(sf::Vertex(rc.GetGrid().GetPoint(point), colour));
	};

	for (auto& floor : m_floors)
	{
		sf::VertexArray verts(sf::Triangles);

		for (auto& point : *floor)
			addPoint(verts, point, sf::Color::Magenta);

		rc.GetWindow().draw(verts);
	}

	for (auto& wall : m_walls)
	{
		sf::VertexArray verts(sf::Triangles);

		for (auto& point : *wall)
			addPoint(verts, point, sf::Color::Red);

		rc.GetWindow().draw(verts);
	}
}

bool VectorObject::AddWall(const Jig::PolyLine& polyline, const Jig::EdgeMesh::Vert& start, const Jig::EdgeMesh::Vert& end)
{
	if (!Jig::EdgeMeshAddFace(*m_edgeMesh, const_cast<Jig::EdgeMesh::Vert&>(start), const_cast<Jig::EdgeMesh::Vert&>(end), polyline))
		return false;

	Update();
	return true;
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

bool VectorObject::Init(std::vector<sf::Vector2f> points)
{
	if (points.empty())
		return false;

	// Polygon::Update() doesn't like duplicate adjacent points.
	points.erase(std::unique(points.begin(), points.end()), points.end());

	if (points.size() < 3)
		return false;

	Jig::Polygon poly;
	poly.reserve(points.size());

	for (const auto& point : points)
		poly.push_back({ (double)point.x, (double)point.y });

	poly.Update();
	if (poly.IsSelfIntersecting())
		return false;

	std::vector<Jig::EdgeMesh::VertPtr> verts;
	verts.reserve(poly.size());
	for (const auto& point : poly)
		verts.push_back(std::make_unique<Jig::EdgeMesh::Vert>(point));

	m_edgeMesh = std::make_unique<Jig::EdgeMesh>(std::move(verts));
	m_edgeMesh->SetEnableVisiblePoints(false);

	auto face = std::make_unique<Jig::EdgeMesh::Face>();
	for (const auto& vert : m_edgeMesh->GetVerts())
		face->AddAndConnectEdge(vert.get());

	m_edgeMesh->AddFace(std::move(face));

	Update();

	return true;
}

void VectorObject::Update()
{
	m_edgeMesh->Update();

	m_floors.clear();
	m_walls.clear();

	for (auto& face : m_edgeMesh->GetFaces())
	{
		auto poly = face->GetPolygon();
		m_floors.push_back(MakeTriangleMesh(Jig::Triangulator(poly).Go()));
		m_walls.push_back(TesselateWall(poly));
	}
}

VectorObject::TriangleMeshPtr VectorObject::TesselateWall(const Jig::Polygon& poly) const
{
	Jig::Polygon inner, outer;

	auto addWallPoints = [&](int index)	
	{
		const Jig::Vec2f prev = poly.GetVertex(index - 1);
		const Jig::Vec2f point = poly.GetVertex(index);
		const Jig::Vec2f next = poly.GetVertex(index + 1);

		auto wallPoints = Jig::GetMitrePoints(point, &prev, &next, ::WallThickness, Jig::LineAlignment::Outer);
		if (!wallPoints.has_value())
			return false;

		inner.push_back(wallPoints.value().first);
		outer.push_back(wallPoints.value().second);
		return true;
	};

	for (size_t i = 0; i < poly.size(); ++i)
		if (!addWallPoints(int(i)))
			break;

	inner.Update();
	outer.Update();
	if (inner.IsSelfIntersecting() || outer.IsSelfIntersecting())
		return nullptr;

	Jig::Triangulator triangulator(outer);
	triangulator.AddHole(inner);

	return MakeTriangleMesh(triangulator.Go());
}

const Jig::EdgeMesh::Vert* VectorObject::FindNearestVert(const sf::Vector2f& point, float tolerance) const
{
	return m_edgeMesh->FindNearestVert(point, tolerance);
}

const Jig::EdgeMesh::Face* VectorObject::HitTestRooms(const sf::Vector2f& point) const
{
	for (auto& face : m_edgeMesh->GetFaces())
		if (face->Contains(point))
			return face.get();

	return nullptr;
}
