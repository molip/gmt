#include "VectorObject.h"
#include "../Grid.h"

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
	Tesselate(points);		  
}

VectorObject::~VectorObject() = default;

void VectorObject::Draw(RenderContext& rc) const
{
	auto addPoint = [&](auto& verts, auto& point, auto colour)
	{
		verts.append(sf::Vertex(rc.GetGrid().GetPoint(point), colour));
	};

	if (m_floor)
	{
		sf::VertexArray floor(sf::Triangles), walls(sf::Triangles);

		for (auto& point : *m_floor)
			addPoint(floor, point, sf::Color::Green);

		if (m_walls)
			for (auto& point : *m_walls)
				addPoint(walls, point, sf::Color::Red);

		rc.GetWindow().draw(floor);
		rc.GetWindow().draw(walls);
	}
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

bool VectorObject::Tesselate(std::vector<sf::Vector2f> points)
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

	std::vector<Jig::EdgeMesh::Vert> verts;
	verts.reserve(poly.size());
	for (const auto& point : poly)
		verts.push_back(point);

	// TODO: EdgeMesh::Face should have vert indices (not ptrs) so we can add verts.
	// Or EdgeMesh::m_verts should be a vector of unique_ptrs.
	m_edgeMesh = std::make_unique<Jig::EdgeMesh>(std::move(verts)); 

	auto face = std::make_unique<Jig::EdgeMesh::Face>();
	for (const auto& vert : m_edgeMesh->GetVerts())
		face->AddAndConnectEdge(&vert);	

	m_edgeMesh->AddFace(std::move(face));
	m_edgeMesh->Update();

	auto poly2 = m_edgeMesh->GetFaces().front()->GetPolygon();
	m_floor = MakeTriangleMesh(Jig::Triangulator(poly2).Go());
	m_walls = TesselateWall(poly2);

	return true;
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

VectorObject::HitTestResult VectorObject::HitTest(const sf::Vector2f& point, float tolerance) const
{
	const float toleranceSquared = tolerance * tolerance;

	float closestDistanceSquared = FLT_MAX;
	const Jig::EdgeMesh::Vert* closestVert{};
	
	for (auto& vert : m_edgeMesh->GetVerts())
	{
		const float distanceSquared = Jig::Vec2f(point - sf::Vector2f(vert)).GetLengthSquared();
		if (distanceSquared < closestDistanceSquared)
		{
			closestVert = &vert;
			closestDistanceSquared = distanceSquared;
		}
	}

	HitTestResult result;
	if (closestDistanceSquared <= toleranceSquared)
	{
		result.distanceSquared = closestDistanceSquared;
		result.vert = closestVert;
	}
	
	return result;
}

const Jig::EdgeMesh::Face* VectorObject::HitTestRooms(const sf::Vector2f& point) const
{
	for (auto& face : m_edgeMesh->GetFaces())
		if (face->Contains(point))
			return face.get();

	return nullptr;
}
