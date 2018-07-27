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

AddVertTool::AddVertTool(const MainView& view) : m_view(view)
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

	if (m_overState.terminus.first)
	{
		drawCircle(sf::Vector2f(m_overState.terminus.second), BigDotRadius, sf::Color::Blue);
	}
}

void AddVertTool::OnMouseMoved(const sf::Vector2i& point)
{
	Update(m_view.DevToLog(point));
}

void AddVertTool::Update(const sf::Vector2f& logPoint)
{
	m_overState = HitTest(logPoint);
}

AddVertTool::OverState AddVertTool::HitTest(const sf::Vector2f& logPoint) const
{
	OverState result;

	float gridDistanceSquared{};
	const sf::Vector2f gridPoint(m_view.GetGrid().GetNearestGridPoint(logPoint, &gridDistanceSquared));

	const Jig::EdgeMesh::Face* hitRoom{};
	for (const auto& object : App::GetModel().GetObjects())
		if (const Model::VectorObject* vector = dynamic_cast<const Model::VectorObject*>(object.get()))
			if (result.room = vector->HitTestRooms(gridPoint))
				break;

	const auto objectPoint = m_view.GetGrid().GetGridPoint(logPoint);

	Kernel::MinFinder<Terminus, float> minFinder(0.5f); // 0.5^2 + 0.5^2

	const Model::VectorObject* hitObject{};

	auto tryObjectEdges = [&](const Model::VectorObject* vectorObject)
	{
		float distSquared{};
		auto result = vectorObject->HitTestEdges(objectPoint, std::sqrt(minFinder.GetValue()), distSquared);
		if (auto* edgeTerm = result.GetEdge())
			if (minFinder.Try(*edgeTerm, distSquared))
				hitObject = vectorObject;
	};

	for (auto& object : App::GetModel().GetObjects())
		if (Model::VectorObject* vectorObject = dynamic_cast<Model::VectorObject*>(object.get()))
			tryObjectEdges(vectorObject);

	result.terminus = minFinder.GetObject();
	result.object = hitObject;

	return result;
}

void AddVertTool::OnMouseDown(sf::Mouse::Button button, const sf::Vector2i & point)
{
	Kernel::Log() << "AddVertTool::OnMouseDown" << std::endl;

	if (!m_overState.terminus.first)
		return;

	auto command = std::make_unique<Model::Command::AddVert>(*m_overState.terminus.first, m_overState.terminus.second, *m_overState.object);
	App::AddCommand(std::move(command));
}
