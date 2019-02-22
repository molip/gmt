#include "SelectTool.h"

#include "../App.h"
#include "../MainView.h"
#include "../RenderContext.h"

#include "../Model/Command/EdgeMesh.h" 
#include "../Model/Command/DeleteVert.h"
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

	if (!m_dragging)
	{
		for (auto& object : App::GetModel().GetObjects())
			if (Model::VectorObject* vectorObject = dynamic_cast<Model::VectorObject*>(object.get()))
				for (auto& vert : vectorObject->GetMesh().GetVerts())
					drawCircle(sf::Vector2f(*vert), SmallDotRadius, sf::Color::Yellow);
	}

	if (m_overState)
		drawCircle(sf::Vector2f(*m_overState->vert), BigDotRadius, sf::Color::Yellow);
}

void SelectTool::OnMouseMoved(const sf::Vector2i& point)
{
	Update(m_view.DevToLog(point));
}

void SelectTool::Update(const sf::Vector2f& logPoint)
{
	if (m_dragging)
	{
		auto& vert = *const_cast<Jig::EdgeMesh::Vert*>(m_overState->vert);
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
	else
	{
		m_overState = std::dynamic_pointer_cast<Model::VertElement>(HitTest(logPoint, nullptr, { HitTestOpt::Verts }));
	}
}

void SelectTool::OnMouseDown(sf::Mouse::Button button, const sf::Vector2i & point)
{
	Kernel::Log() << "SelectTool::OnMouseDown dev=" << point << std::endl;

	auto* vert = m_overState ? m_overState->vert : nullptr;
	if (vert != App::GetSelection().GetVert())
	{
		Model::Selection selection;
		if (vert)
			selection = Model::Selection(*m_overState->GetObject(), *vert);

		App::SetSelection(selection);
	}

	m_dragging = vert;
}

void SelectTool::OnMouseUp(sf::Mouse::Button button, const sf::Vector2i & point)
{
	Kernel::Log() << "SelectTool::OnMouseUp dev=" << point << std::endl;

	if (m_command)
		App::AddCommand(std::move(m_command), true);
	
	m_dragging = false;
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
