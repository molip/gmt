#include "VectorTool.h"

#include "../App.h"
#include "../MainView.h"
#include "../RenderContext.h"

#include "../Model/Model.h"

#include "Jig/PolyLine.h"

#include "libKernel/MinFinder.h"

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

	sf::VertexArray verts(sf::LinesStrip);

	for (auto& point : m_points)
	{
		verts.append(sf::Vertex(rc.GetGrid().GetPoint(point), sf::Color::Green));
		drawCircle(point, SmallDotRadius, sf::Color::Green);
	}

	if (!m_points.empty())
		verts.append(sf::Vertex(rc.GetGrid().GetPoint(m_overState.gridPoint), sf::Color::Green));

	rc.GetWindow().draw(verts);

	if (m_overState.snap != Snap::None)
	{
		auto colour =
			m_overState.snap == Snap::Edge ? sf::Color::Blue :
			m_overState.snap == Snap::Vert ? sf::Color::Yellow :
			IsClosed() ? sf::Color::Green :
			sf::Color::Red;
	
		drawCircle(m_overState.gridPoint, BigDotRadius, colour);
	}
}

bool VectorTool::IsClosed() const
{
	return m_points.size() >= 3 && m_overState.snap != Snap::None && m_overState.gridPoint == m_points.front();
}

void VectorTool::OnMouseMoved(const sf::Vector2i& point)
{
	Update(m_view.DevToLog(point));
}

void VectorTool::Update(const sf::Vector2f& logPoint)
{
	if (m_objectEdit)
	{
		m_overState = HitTest(logPoint, m_objectEdit->object);
		if (m_overState.terminus == m_objectEdit->start) // Back where we started.
			m_overState.snap = Snap::None;

		if (m_points.size() == 1) // Just started, don't know if internal/external yet. 
		{
			m_objectEdit->room = m_overState.room;
		}
		else if (m_overState.snap == Snap::Grid && m_overState.room != m_objectEdit->room)
		{
			m_overState.snap = Snap::None;
		}

		// TODO: Check polyline is completely in room (or completely outside).
	}
	else
	{
		m_overState = HitTest(logPoint, nullptr);

		if (m_overState.snap == Snap::Grid && m_overState.room) // Can't start a new object in a room.
			m_overState.snap = Snap::None;
	}

	if (m_overState.snap == Snap::None)
		m_overState.gridPoint = m_view.GetGrid().GetGridPoint(logPoint);
}

VectorTool::OverState VectorTool::HitTest(const sf::Vector2f& logPoint, const Model::VectorObject* special) const
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

	using HitGrid = sf::Vector2f;
	using HitResult = std::variant<std::monostate, Terminus, HitGrid>;
	Kernel::MinFinder<HitResult, float> minFinder(0.5f); // 0.5^2 + 0.5^2

	const Model::VectorObject* hitObject{};

	auto tryObjectEdges = [&](const Model::VectorObject* vectorObject)
	{
		float distSquared{};
		auto result = vectorObject->HitTestEdges(objectPoint, std::sqrt(minFinder.GetValue()), distSquared);
		if (minFinder.Try(result, distSquared))
			hitObject = vectorObject;
	};

	if (special)
		tryObjectEdges(special);
	else
		for (auto& object : App::GetModel().GetObjects())
			if (Model::VectorObject* vectorObject = dynamic_cast<Model::VectorObject*>(object.get()))
				tryObjectEdges(vectorObject);

	minFinder.Try(gridPoint, gridDistanceSquared);

	auto& results = minFinder.GetObject();
	if (auto terminusResult = std::get_if<Terminus>(&results))
	{
		result.terminus = *terminusResult;
		result.object = hitObject;
		result.gridPoint = sf::Vector2f(*terminusResult->GetPoint());
		result.snap = terminusResult->GetEdge() ? Snap::Edge : Snap::Vert;
	}
	else if (auto gridResult = std::get_if<HitGrid>(&results))
	{
		result.gridPoint = gridPoint;
		result.snap = Snap::Grid;
	}

	return result;
}

void VectorTool::OnMouseDown(sf::Mouse::Button button, const sf::Vector2i & point)
{
	if (m_overState.snap == Snap::None)
		return;

	bool finished{};

	if (m_points.empty() && m_overState.object)
	{
		m_objectEdit = std::make_unique<ObjectEdit>();
		m_objectEdit->object = m_overState.object;
		m_objectEdit->start = m_overState.terminus;
	}
	else if (m_objectEdit)
	{
		if (finished = m_overState.snap == Snap::Edge || m_overState.snap == Snap::Vert)
		{
			m_points.erase(m_points.begin());
			
			Jig::PolyLine poly;
			for (auto& point : m_points)
				poly.push_back(Jig::Vec2(point));

			const_cast<Model::VectorObject*>(m_objectEdit->object)->AddWall(poly, m_objectEdit->start, m_overState.terminus);
			
			m_points.clear();
			m_objectEdit = nullptr;
		}
	}
	else if (IsClosed())
	{
		auto object = std::make_unique<Model::VectorObject>(m_points);
		App::GetModel().AddObject(std::move(object));
		finished = true;
		m_points.clear();
	}

	if (!finished)
		m_points.push_back(m_overState.gridPoint);
}