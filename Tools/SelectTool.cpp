#include "SelectTool.h"

#include "../App.h"
#include "../MainView.h"
#include "../RenderContext.h"

#include "../Model/Command/AddVert.h"
#include "../Model/Command/EdgeMesh.h" 
#include "../Model/Command/DeleteVert.h"
#include "../Model/Command/MoveWallThing.h"
#include "../Model/Model.h"
#include "../Model/Selection.h"

#include "Jig/EdgeMeshCommand.h"

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

SelectTool::SelectTool(const MainView& view) : Tool(view)
{
}

void SelectTool::Draw(RenderContext& rc) const
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

	if (auto* selectedVert = App::GetSelection().GetVert())
		drawCircle(sf::Vector2f(*selectedVert), BigDotRadius, sf::Color::Yellow, true);

	if (m_dragging)
	{
		drawCircle(m_overState->GetPointF(), BigDotRadius, sf::Color::Yellow);
	}
	else
	{
		for (auto& object : App::GetModel().GetObjects())
			if (Model::VectorObject* vectorObject = dynamic_cast<Model::VectorObject*>(object.get()))
				for (auto& vert : vectorObject->GetMesh().GetVerts())
					drawCircle(sf::Vector2f(*vert), SmallDotRadius, sf::Color::Yellow);

		if (m_overState)
		{
			const sf::Color colour = m_overState->GetAs<Model::EdgeElement>() ? sf::Color::Blue : sf::Color::Yellow;
			drawCircle(m_overState->GetPointF(), BigDotRadius, colour);
		}
	}
}

void SelectTool::OnMouseMoved(const sf::Vector2i& point)
{
	Update(m_view.DevToLog(point));
}

void SelectTool::Update(const sf::Vector2f& logPoint)
{
	if (m_dragging)
	{
		if (auto* element = m_overState->GetAs<Model::VertElement>())
		{
			auto& vert = *const_cast<Jig::EdgeMesh::Vert*>(element->vert);
			auto snapped = Jig::Vec2(m_view.GetGrid().GetNearestGridPoint(logPoint, nullptr));

			if (vert != snapped)
			{
				if (m_command)
					m_command->UndoNoUpdate();

				auto moveCommand = std::make_unique<Jig::EdgeMeshCommand::MoveVert>(vert, snapped);
				m_command = std::make_unique<Model::Command::EdgeMesh>(std::move(moveCommand), *m_overState->GetObject());
				App::DoCommand(*m_command);
			}
		}
		else if (auto* element = m_overState->GetAs<Model::WallThingElement>())
		{
			auto hit = HitTest(logPoint, element->object, { HitTester::Option::Edges }, false);
			if (auto& hitElement = std::dynamic_pointer_cast<Model::EdgeElement>(hit))
			{
				if (m_command)
					m_command->UndoNoUpdate();

				const Model::WallThing::Position position = { hitElement->edge, hitElement->normalisedDist };
				m_command = std::make_unique<Model::Command::MoveWallThing>(*element->thing, position);
				App::DoCommand(*m_command);
			}
		}

	}
	else
	{
		using Opt = HitTester::Option;
		m_overState = HitTest(logPoint, nullptr, { Opt::Verts, Opt::EdgePoints, Opt::WallThings });
	}
}

void SelectTool::OnMouseDown(sf::Mouse::Button button, const sf::Vector2i & point)
{
	Kernel::Log() << "SelectTool::OnMouseDown dev=" << point << std::endl;

	const Jig::EdgeMesh::Vert* vert{};

	if (auto* element = m_overState->GetAs<Model::EdgeElement>())
	{
		auto command = std::make_unique<Model::Command::AddVert>(*element->edge, element->GetPoint(), *element->object);
		auto* save = command.get();
		App::AddCommand(std::move(command));

		m_overState = std::make_unique<Model::VertElement>(element->object, save->GetNewVert());
		vert = save->GetNewVert();
	}
	
	if (auto* element = m_overState->GetAs<Model::VertElement>())
	{
		if (element->vert != App::GetSelection().GetVert())
		{
			Model::Selection selection;
			if (element->vert)
				selection = Model::Selection(*m_overState->GetObject(), *element->vert);

			App::SetSelection(selection);
		}
	}

	m_dragging = m_overState != nullptr;
}

void SelectTool::OnMouseUp(sf::Mouse::Button button, const sf::Vector2i & point)
{
	Kernel::Log() << "SelectTool::OnMouseUp dev=" << point << std::endl;

	m_dragging = false;

	if (m_command)
		App::AddCommand(std::move(m_command), true);

	Update(m_view.DevToLog(point));
}

void SelectTool::OnKeyPressed(const sf::Event::KeyEvent event)
{
	if (event.code == sf::Keyboard::Delete)
	{
		if (auto* vert = App::GetSelection().GetVert())
		{
			auto command = std::make_unique<Model::Command::DeleteVert>(*vert, *App::GetSelection().GetObject()); 
			if (command->CanDo())
				App::AddCommand(std::move(command));
		}
	}
}
