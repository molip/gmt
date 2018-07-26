#include "SelectTool.h"

#include "../App.h"
#include "../MainView.h"
#include "../RenderContext.h"

#include "../Model/Command/EdgeMesh.h" 
#include "../Model/Model.h"
#include "../Model/Selection.h"

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

SelectTool::SelectTool(const MainView& view) : m_view(view)
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

	if (m_overState.vert)
		drawCircle(sf::Vector2f(*m_overState.vert), BigDotRadius, sf::Color::Yellow);
}

void SelectTool::OnMouseMoved(const sf::Vector2i& point)
{
	Update(m_view.DevToLog(point));
}

void SelectTool::Update(const sf::Vector2f& logPoint)
{
	m_gridPoint = m_view.GetGrid().GetGridPoint(logPoint);

	if (m_dragging)
	{
		auto& vert = *const_cast<Jig::EdgeMesh::Vert*>(m_overState.vert);
		auto snapped = Jig::Vec2(m_view.GetGrid().GetNearestGridPoint(logPoint, nullptr));
		
		if (vert != snapped)
		{
			if (m_command)
				m_command->UndoNoUpdate();

			auto moveCommand = std::make_unique<Jig::EdgeMeshCommand::MoveVert>(vert, snapped);
			m_command = std::make_unique<Model::Command::EdgeMesh>(std::move(moveCommand), *m_overState.object);
			App::DoCommand(*m_command);
		}
	}
	else
	{
		Kernel::MinFinder<OverState, float> minFinder(0.5f); // 0.5^2 + 0.5^2

		for (auto& object : App::GetModel().GetObjects())
		{
			if (Model::VectorObject* vectorObject = dynamic_cast<Model::VectorObject*>(object.get()))
			{
				float distSquared{};
				auto result = vectorObject->HitTestEdges(m_gridPoint, std::sqrt(minFinder.GetValue()), distSquared);
				if (auto vert = result.GetVert())
					minFinder.Try({ *vert, vectorObject }, distSquared);
			}
		}

		m_overState = minFinder.GetObject();
	}
}

void SelectTool::OnMouseDown(sf::Mouse::Button button, const sf::Vector2i & point)
{
	Kernel::Log() << "SelectTool::OnMouseDown dev=" << point << std::endl;

	if (m_overState.vert != App::GetSelection().GetVert())
	{
		Model::Selection selection;
		if (m_overState.vert)
			selection.SetVert(*m_overState.vert);

		App::SetSelection(selection);
	}

	m_dragging = m_overState.vert;
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
}
