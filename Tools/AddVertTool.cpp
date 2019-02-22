#include "AddVertTool.h"

#include "../App.h"
#include "../MainView.h"
#include "../RenderContext.h"

#include "../Model/Command/AddVert.h"
#include "../Model/Model.h"

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

AddVertTool::AddVertTool(const MainView& view) : Tool(view)
{
}

void AddVertTool::Draw(RenderContext& rc) const
{
	auto drawCircle = [&](sf::Vector2f& gridPoint, float radius, auto& colour)
	{
		sf::CircleShape circle(radius);
		circle.setFillColor(colour);
		circle.setPosition(m_view.GetGrid().GetPoint(gridPoint) - sf::Vector2f(radius, radius));
		rc.GetWindow().draw(circle);
	};

	for (auto& object : App::GetModel().GetObjects())
		if (Model::VectorObject* vectorObject = dynamic_cast<Model::VectorObject*>(object.get()))
			for (auto& vert : vectorObject->GetMesh().GetVerts())
				drawCircle(sf::Vector2f(*vert), SmallDotRadius, sf::Color::Yellow);

	if (m_overState)
	{
		drawCircle(sf::Vector2f(m_overState->point), BigDotRadius, sf::Color::Blue);
	}
}

void AddVertTool::OnMouseMoved(const sf::Vector2i& point)
{
	Update(m_view.DevToLog(point));
}

void AddVertTool::Update(const sf::Vector2f& logPoint)
{
	m_overState = std::dynamic_pointer_cast<Model::EdgePointElement>(HitTest(logPoint, nullptr, { HitTestOpt::Edges }));
}

void AddVertTool::OnMouseDown(sf::Mouse::Button button, const sf::Vector2i & point)
{
	Kernel::Log() << "AddVertTool::OnMouseDown" << std::endl;

	if (!m_overState)
		return;

	auto command = std::make_unique<Model::Command::AddVert>(*m_overState->edge, m_overState->point, *m_overState->GetObject());
	App::AddCommand(std::move(command));
}
