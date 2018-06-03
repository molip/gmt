#include "VectorObject.h"
#include "../Grid.h"

#include <SFML/Graphics.hpp>

using namespace GMT;
using namespace GMT::Model;

VectorObject::VectorObject()
{
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

	rc.GetWindow().draw(lines);
}

bool VectorObject::IsClosed() const
{
	return m_points.size() > 1 && m_points.back() == m_points.front();
}
