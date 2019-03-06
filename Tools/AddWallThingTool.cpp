#include "AddWallThingTool.h"

#include "../App.h"
#include "../MainView.h"
#include "../RenderContext.h"

#include "../Model/Command/AddWallThing.h"
#include "../Model/Model.h"

#include <SFML/Graphics.hpp>

using namespace GMT;
using namespace GMT::Tools;

namespace
{
	const float BigDotRadius = 10; // Logical.
	const float SmallDotRadius = 5; // Logical.
}

AddWallThingTool::AddWallThingTool(const MainView& view) : Tool(view)
{
}

void AddWallThingTool::Draw(RenderContext& rc) const
{
	auto drawCircle = [&](sf::Vector2f& gridPoint, float radius, auto& colour, bool outline = false)
	{
		sf::CircleShape circle(radius);
		circle.setFillColor(outline ? sf::Color::Transparent : colour);
		if (outline)
		{
			circle.setOutlineColor(colour);
			circle.setOutlineThickness(1);
		}
		circle.setPosition(m_view.GetGrid().GetPoint(gridPoint) - sf::Vector2f(radius, radius));
		rc.GetWindow().draw(circle);
	};

	for (auto& object : App::GetModel().GetObjects())
		if (Model::VectorObject* vectorObject = dynamic_cast<Model::VectorObject*>(object.get()))
			for (auto& vert : vectorObject->GetMesh().GetVerts())
				drawCircle(sf::Vector2f(*vert), SmallDotRadius, sf::Color::Yellow);

	if (m_overState)
	{
		drawCircle(m_overState->GetPointF(), BigDotRadius, sf::Color::Magenta);
	}
}

void AddWallThingTool::OnMouseMoved(const sf::Vector2i& point)
{
	Update(m_view.DevToLog(point));
}

void AddWallThingTool::Update(const sf::Vector2f& logPoint)
{
	using Opt = HitTester::Option;
	m_overState = HitTest(logPoint, nullptr, { Opt::Edges });
}

void AddWallThingTool::OnMouseDown(sf::Mouse::Button button, const sf::Vector2i & point)
{
	if (!m_overState)
		return;

	auto* element = m_overState->GetAs<Model::EdgeElement>();
	auto thing = std::make_unique<Model::WallThing>();
	thing->SetPosition({ element->edge, element->normalisedDist });

	auto command = std::make_unique<Model::Command::AddWallThing>(*element->object, std::move(thing));
	App::AddCommand(std::move(command));
}

void AddWallThingTool::OnMouseUp(sf::Mouse::Button button, const sf::Vector2i & point)
{
	Update(m_view.DevToLog(point));
}

void AddWallThingTool::OnKeyPressed(const sf::Event::KeyEvent event)
{
}
