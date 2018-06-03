#include "VectorObject.h"
#include "../Grid.h"

#include "Jig/Polygon.h"
#include "Jig/Triangulator.h"

#include <SFML/Graphics.hpp>

using namespace GMT;
using namespace GMT::Model;

VectorObject::VectorObject()
{
}

VectorObject::~VectorObject() = default;

void VectorObject::Draw(RenderContext& rc) const
{
	sf::VertexArray verts;
	auto addPoint = [&](auto& point)
	{
		verts.append(sf::Vertex(rc.GetGrid().GetPoint(point), sf::Color::Green));
	};

	if (m_tesselated)
	{
		verts.setPrimitiveType(sf::Triangles);
		for (auto& point : *m_tesselated)
			addPoint(point);
	}
	else
	{
		verts.setPrimitiveType(sf::LinesStrip);
		for (auto& point : m_points)
			addPoint(point);
	}

	rc.GetWindow().draw(verts);
}

bool VectorObject::IsClosed() const
{
	return m_points.size() > 1 && m_points.back() == m_points.front();
}

bool VectorObject::Tesselate()
{
	//auto sorted = m_points;
	//std::sort(sorted.begin(), sorted.end() - 1, [](auto& lhs, auto& rhs) { return lhs.x == rhs.x ? lhs.y < rhs.y : lhs.x < rhs.x; });
	
	//if (std::adjacent_find(m_points.begin(), m_points.end()) != m_points.end())
	//	return;

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

