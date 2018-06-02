#include "VectorObject.h"
#include "../Grid.h"

#include <SFML/Graphics.hpp>

using namespace GMT;
using namespace GMT::Model;

VectorObject::VectorObject()
{
	m_points = { { 1, 5 }, { 5, 1 }, { 9, 5 }, { 5, 9 } };
}

VectorObject::~VectorObject() = default;

void VectorObject::Draw(RenderContext& rc) const
{
	sf::VertexArray lines(sf::LinesStrip);

	auto addPoint = [&](auto& point)
	{
		lines.append(sf::Vertex(rc.GetGrid().GetPoint(point), sf::Color::Green));
	};

	for (auto& point : m_points)
		addPoint(point);

	if (!m_points.empty())
		addPoint(m_points.front());

	rc.GetWindow().draw(lines);
}