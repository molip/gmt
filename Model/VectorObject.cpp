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

VectorObject::VectorObject()
{
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
	else
	{
		sf::VertexArray verts(sf::LinesStrip);

		for (auto& point : m_points)
			addPoint(verts, point, sf::Color::Green);

		rc.GetWindow().draw(verts);
	}
}

bool VectorObject::IsClosed() const
{
	return m_points.size() > 1 && m_points.back() == m_points.front();
}

VectorObject::MeshPtr VectorObject::MakeMesh(const Jig::EdgeMesh& edgeMesh) const
{
	MeshPtr mesh = std::make_unique<Mesh>();
	mesh->reserve(edgeMesh.GetFaces().size() * 3);
	for (auto& face : edgeMesh.GetFaces())
		for (auto& point : face->GetPointLoop())
			mesh->push_back({ (float)point.x, (float)point.y });

	return mesh;
}

bool VectorObject::Tesselate()
{
	if (m_points.empty())
		return false;

	m_points.pop_back();

	// Polygon::Update() doesn't like duplicate adjacent points.
	m_points.erase(std::unique(m_points.begin(), m_points.end()), m_points.end());

	if (m_points.size() < 3)
		return false;

	Jig::Polygon poly;
	poly.reserve(m_points.size());

	for (auto& point : m_points)
		poly.push_back({ (double)point.x, (double)point.y });

	poly.Update();
	if (poly.IsSelfIntersecting())
		return false;

	m_floor = MakeMesh(Jig::Triangulator(poly).Go());
	m_walls = TesselateWall(poly);

	return true;
}

VectorObject::MeshPtr VectorObject::TesselateWall(const Jig::Polygon& poly) const
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

	return MakeMesh(triangulator.Go());
}
