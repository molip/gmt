#include "DeleteTool.h"

#include "../App.h"
#include "../MainView.h"
#include "../RenderContext.h"

#include "../Model/Command/DeleteFace.h"
#include "../Model/Command/DeleteVert.h"
#include "../Model/Command/DeleteWallThing.h"
#include "../Model/Command/MergeFace.h"
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

DeleteTool::DeleteTool(const MainView& view) : Tool(view)
{
}

void DeleteTool::Draw(RenderContext& rc) const
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
		if (auto* element = m_overState->GetAs<Model::VertElement>())
		{
			drawCircle(sf::Vector2f(*element->vert), BigDotRadius, sf::Color::Red);
		}
		if (auto* element = m_overState->GetAs<Model::WallThingElement>())
		{
			drawCircle(sf::Vector2f(element->GetPoint()), BigDotRadius, sf::Color::Red);
		}
		else if (auto* element = m_overState->GetAs<Model::EdgeElement>())
		{
			drawCircle(sf::Vector2f(*element->edge->vert), BigDotRadius, sf::Color::Red);
			drawCircle(sf::Vector2f(*element->edge->next->vert), BigDotRadius, sf::Color::Red);
		}
		else if (auto* element = m_overState->GetAs<Model::FaceElement>())
		{
			for (auto& edge : element->face->GetEdges())
				drawCircle(sf::Vector2f(*edge.vert), BigDotRadius, sf::Color::Red);
		}
	}
}

void DeleteTool::OnMouseMoved(const sf::Vector2i& point)
{
	Update(m_view.DevToLog(point));
}

void DeleteTool::Update(const sf::Vector2f& logPoint)
{
	using Opt = HitTester::Option;
	m_overState = HitTest(logPoint, nullptr, { Opt::Verts, Opt::InternalEdges, Opt::Faces, Opt::WallThings });
}

void DeleteTool::OnMouseDown(sf::Mouse::Button button, const sf::Vector2i & point)
{
	if (!m_overState)
		return;

	Model::Command::BasePtr command;

	if (auto* element = m_overState->GetAs<Model::VertElement>())
		command = std::make_unique<Model::Command::DeleteVert>(*element->vert, *element->object);
	else if (auto* element = m_overState->GetAs<Model::WallThingElement>())
		command = std::make_unique<Model::Command::DeleteWallThing>(*element->thing, *element->object);
	else if (auto* element = m_overState->GetAs<Model::EdgeElement>())
		command = std::make_unique<Model::Command::MergeFace>(*element->edge, *element->object);
	else if (auto* element = m_overState->GetAs<Model::FaceElement>())
		command = std::make_unique<Model::Command::DeleteFace>(*element->face, *element->object);

	if (command->CanDo())
	{
		App::AddCommand(std::move(command));
		Update(m_view.DevToLog(point));
	}
}
