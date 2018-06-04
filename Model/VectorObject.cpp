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

VectorObject::WallPoints VectorObject::GetWallPointsLog(int index) const
{
	if (m_points.empty())
		return {};

	const Jig::Vec2f prev(m_points[(index + m_points.size() - 1) % m_points.size()]);
	const Jig::Vec2f point(m_points[index % m_points.size()]);
	const Jig::Vec2f next(m_points[(index + 1) % m_points.size()]);

	const auto* pPrev = (m_tesselated || index > 0) ? &prev : nullptr;
	const auto* pNext = (m_tesselated || index < m_points.size() - 1) ? &next : nullptr;

	return Jig::GetMitrePoints(point, pPrev, pNext, ::WallThickness, Jig::LineAlignment::Outer);
}

void VectorObject::Draw(RenderContext& rc) const
{
	auto addPoint = [&](auto& verts, auto& point)
	{
		verts.append(sf::Vertex(rc.GetGrid().GetPoint(point), sf::Color::Green));
	};

	sf::VertexArray verts;
	if (m_tesselated)
	{
		verts.setPrimitiveType(sf::Triangles);
		for (auto& point : *m_tesselated)
			addPoint(verts, point);
	}
	else
	{
		verts.setPrimitiveType(sf::LinesStrip);
		for (auto& point : m_points)
			addPoint(verts, point);
	}

	rc.GetWindow().draw(verts);

	sf::VertexArray normals(sf::Lines);
	sf::VertexArray wall1(sf::LinesStrip);
	sf::VertexArray wall2(sf::LinesStrip);

	auto addWallPoints = [&](int index)
	{
		auto wallPoints = GetWallPointsLog(index);
		if (!wallPoints.has_value())
			return false;

		const sf::Vector2f dev1 = rc.GetGrid().GetPoint(wallPoints.value().first);
		const sf::Vector2f dev2 = rc.GetGrid().GetPoint(wallPoints.value().second);

		normals.append(sf::Vertex(dev1, sf::Color::Red));
		normals.append(sf::Vertex(dev2, sf::Color::Red));

		wall1.append(sf::Vertex(dev1, sf::Color::Red));
		wall2.append(sf::Vertex(dev2, sf::Color::Red));
		return true;
	};

	for (int i = 0; i < (int)m_points.size(); ++i)
		if (!addWallPoints(i))
			break;

	if (m_tesselated)
		addWallPoints(0);

	rc.GetWindow().draw(normals);
	rc.GetWindow().draw(wall1);
	rc.GetWindow().draw(wall2);

}

bool VectorObject::IsClosed() const
{
	return m_points.size() > 1 && m_points.back() == m_points.front();
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

	Jig::EdgeMesh mesh = Jig::Triangulator(poly).Go();

	m_tesselated = std::make_unique<std::vector<sf::Vector2i>>();
	m_tesselated->reserve(mesh.GetFaces().size() * 3);

	for (auto& face : mesh.GetFaces())
		for (auto& point : face->GetPointLoop())
			m_tesselated->push_back({ (int)point.x, (int)point.y });

	return true;
}
