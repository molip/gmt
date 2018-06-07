#include "VectorTool.h"

#include "../App.h"
#include "../MainView.h"
#include "../RenderContext.h"

#include "../Model/Model.h"
#include "../Model/VectorObject.h"

#include <SFML/Graphics.hpp>

using namespace GMT;
using namespace GMT::Tools;

namespace
{
	const int SnappingDistance = 100; // Device.
	const float BigDotRadius = 10; // Logical.
	const float SmallDotRadius = 5; // Logical.
}

VectorTool::VectorTool(const MainView& view) : m_view(view)
{
}

void VectorTool::Draw(RenderContext& rc) const
{
	auto drawCircle = [&](auto& gridPoint, float radius, auto& colour)
	{
		sf::CircleShape circle(radius);
		circle.setFillColor(colour);
		circle.setPosition(m_view.GetGrid().GetPoint(gridPoint) - sf::Vector2f(radius, radius));
		rc.GetWindow().draw(circle);
	};

	if (m_object)
		m_object->Draw(rc);

	if (m_object)
		for (auto& point : m_object->GetPoints())
		drawCircle(point, SmallDotRadius, sf::Color::Green);

	if (m_snapped)
		drawCircle(m_gridPoint, BigDotRadius, m_object && m_object->IsClosed() ? sf::Color::Green : sf::Color::Red);
}

void VectorTool::OnMouseMoved(const sf::Vector2i& point)
{
	sf::Vector2f logPoint = m_view.DevToLog(point);

	float distanceSquared{};
	m_gridPoint = m_view.GetGrid().GetNearestGridPoint(logPoint, &distanceSquared);
	m_snapped = distanceSquared < SnappingDistance * SnappingDistance * m_view.GetDevToLogScale();

	if (m_snapped && m_object && !m_object->GetPoints().empty() && m_object->GetPoints().back() != m_gridPoint)
	{
		m_object->GetPoints().back() = m_gridPoint;
	}
}

void VectorTool::OnMouseDown(sf::Mouse::Button button, const sf::Vector2i & point)
{
	if (!m_snapped)
		return;

	if (m_object)
	{
		if (m_object->IsClosed())
		{
			auto object = std::move(m_object);
			if (object->Tesselate())
				App::GetModel().AddObject(std::move(object));
		}
		else
			m_object->GetPoints().push_back(m_gridPoint);
	}
	else
	{
		m_object = std::make_unique<Model::VectorObject>();
		m_object->GetPoints().push_back(m_gridPoint);
		m_object->GetPoints().push_back(m_gridPoint);
	}
}
