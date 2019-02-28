#include "VectorTool.h"

#include "../App.h"
#include "../MainView.h"
#include "../RenderContext.h"

#include "../Model/Command/AddObject.h"
#include "../Model/Command/AddWall.h" 
#include "../Model/Model.h"

#include "Jig/EdgeMeshAddFace.h"
#include "Jig/EdgeMeshCommand.h"
#include "Jig/PolyLine.h"

#include "libKernel/Log.h"
#include "libKernel/MinFinder.h"

#include <SFML/Graphics.hpp>

using namespace GMT;
using namespace GMT::Tools;

namespace
{
	const float BigDotRadius = 10; // Logical.
	const float SmallDotRadius = 5; // Logical.
}

VectorTool::VectorTool(const MainView& view) : Tool(view)
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

	sf::VertexArray verts(sf::LinesStrip);

	for (auto& point : m_points)
	{
		verts.append(sf::Vertex(rc.GetGrid().GetPoint(point), sf::Color::Green));
		drawCircle(point, SmallDotRadius, sf::Color::Green);
	}

	if (!m_points.empty() && m_overState)
		verts.append(sf::Vertex(rc.GetGrid().GetPoint(m_overState->GetPoint()), sf::Color::Green));

	rc.GetWindow().draw(verts);

	if (m_overState)
	{
		auto colour =
			m_overState->GetAs<Model::EdgePointElement>() ? sf::Color::Blue :
			m_overState->GetAs<Model::VertElement>() ? sf::Color::Yellow :
			IsClosed() ? sf::Color::Green :
			sf::Color::Red;
	
		drawCircle(m_overState->GetPoint(), BigDotRadius, colour);
	}
}

bool VectorTool::IsClosed() const
{
	return m_points.size() >= 3 && m_overState && m_overState->GetPoint() == m_points.front();
}

void VectorTool::OnMouseMoved(const sf::Vector2i& point)
{
	Update(m_view.DevToLog(point));
}

void VectorTool::Update(const sf::Vector2f& logPoint)
{
	if (m_objectEdit)
	{
		using Opt = HitTester::Option;
		m_overState = HitTest(logPoint, m_objectEdit->object, { Opt::Verts, Opt::EdgePoints, Opt::Grid });
		if (m_overState == m_objectEdit->start) // Back where we started.
			m_overState.reset();

		if (auto* gridElement = m_overState->GetAs<Model::GridElement>())
		{
			if (m_points.size() == 1) // Just started, don't know if internal/external yet. 
			{
				m_objectEdit->room = gridElement->face;
			}
			else if (gridElement->face != m_objectEdit->room)
			{
				m_overState.reset();
			}
		}

		// TODO: Check polyline is completely in room (or completely outside).
	}
	else
	{
		using Opt = HitTester::Option;
		m_overState = HitTest(logPoint, nullptr, { Opt::Verts, Opt::EdgePoints, Opt::Grid });

		if (auto* gridElement = m_overState->GetAs<Model::GridElement>())
			if (gridElement->face) // Can't start a new object in a room.
				m_overState.reset();
	}
}

bool VectorTool::AddWall(const Jig::PolyLine& polyline, Model::ElementPtr start, Model::ElementPtr end)
{
	auto command = std::make_unique<Model::Command::AddWall>(std::move(start), std::move(end), polyline, *m_objectEdit->object);
	if (!command->CanDo())
		return false;

	App::AddCommand(std::move(command));
	return true;
}

void VectorTool::OnMouseDown(sf::Mouse::Button button, const sf::Vector2i & point)
{
	if (!m_overState)
		return;

	bool finished{};

	if (m_points.empty() && m_overState->GetObject())
	{
		m_objectEdit = std::make_unique<ObjectEdit>();
		m_objectEdit->object = m_overState->GetObject();
		m_objectEdit->start = m_overState;
	}
	else if (m_objectEdit)
	{
		if (finished = m_overState->GetAs<Model::EdgePointElement>() || m_overState->GetAs<Model::VertElement>())
		{
			m_points.erase(m_points.begin());
			
			Jig::PolyLine poly;
			for (auto& point : m_points)
				poly.push_back(Jig::Vec2(point));

			AddWall(poly, m_objectEdit->start, m_overState);
			
			m_points.clear();
			m_objectEdit = nullptr;
		}
	}
	else if (IsClosed())
	{
		if (auto& object = Model::VectorObject::Create(m_points))
		{
			Kernel::Log(1) << "Creating object with " << m_points.size() << " points" << std::endl;
			App::AddCommand(std::make_unique<Model::Command::AddObject>(std::move(object)));
		}

		m_points.clear();
		finished = true;
	}

	if (!finished && m_overState)
		m_points.push_back(m_overState->GetPoint());
}

void VectorTool::OnKeyPressed(const sf::Event::KeyEvent event)
{
	if (m_points.empty())
		return;

	if (event.code == sf::Keyboard::Escape)
		m_points.clear();
	else if (event.code == sf::Keyboard::BackSpace)
		m_points.pop_back();
	else
		return;

	if (m_points.empty())
		m_objectEdit = nullptr;
}
