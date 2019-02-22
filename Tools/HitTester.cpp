#include "HitTester.h"

#include "../App.h"
#include "../MainView.h"

#include "../Model/Model.h"
#include "../Model/VectorObject.h"

#include "libKernel/Math.h"

using namespace GMT;
using namespace GMT::Tools;

HitTester::HitTester(const MainView& view, const Options& opts) : m_view(view), m_opts(opts)
{
}

Model::ElementPtr HitTester::HitTest(const sf::Vector2f& logPoint, const Model::VectorObject* special) const
{
	const auto objectPoint = m_view.GetGrid().GetGridPoint(logPoint);

	const float threshold = std::sqrtf(0.5f * 0.5f * 2);
	Kernel::MinFinder<Model::ElementPtr, float> minFinder(threshold * threshold);

	if (m_opts[Option::Edges] || m_opts[Option::Verts])
	{
		if (special)
			HitTestObject(*special, objectPoint, minFinder, threshold);
		else
			for (auto& object : App::GetModel().GetObjects())
				if (Model::VectorObject* vectorObject = dynamic_cast<Model::VectorObject*>(object.get()))
					HitTestObject(*vectorObject, objectPoint, minFinder, threshold);
	}

	if (m_opts[Option::Grid])
	{
		float gridDistanceSquared{};
		const sf::Vector2f gridPoint(m_view.GetGrid().GetNearestGridPoint(logPoint, &gridDistanceSquared));

		const Jig::EdgeMesh::Face* gridRoom{};
		for (const auto& object : App::GetModel().GetObjects())
			if (const Model::VectorObject* vector = dynamic_cast<const Model::VectorObject*>(object.get()))
				if (gridRoom = vector->GetMesh().HitTest(Jig::Vec2(gridPoint)))
					break;
		minFinder.Try(std::make_shared<Model::GridElement>(gridRoom, gridPoint), gridDistanceSquared);
	}

	return minFinder.GetObject();
}

void HitTester::HitTestEdge(const Model::VectorObject& object, const sf::Vector2f& point, const Jig::EdgeMesh::Edge& edge, const Jig::EdgeMesh::Edge& nextEdge, MinFinder& minFinder, float threshold) const
{
	const auto line = Jig::Line2::MakeFinite(*edge.vert, *nextEdge.vert);
	Jig::Rect bbox = line.GetBBox();
	bbox.Inflate(threshold, threshold);
	if (!bbox.Contains(point))
		return;

	Jig::Vec2 intersect;
	if (line.PerpIntersect(point, nullptr, &intersect))
	{
		if (!line.IsVertical())
		{
			double x = int(intersect.x + 0.5);
			if (x > bbox.m_p0.x && x < bbox.m_p1.x && !Kernel::fcomp(x, line.GetP0().x) && !Kernel::fcomp(x, line.GetP1().x))
			{
				const Jig::Vec2 snapped(x, intersect.y + (x - intersect.x) * line.GetGradient());
				minFinder.Try(std::make_shared<Model::EdgePointElement>(&object, &edge, snapped), Jig::Vec2f(Jig::Vec2f(snapped) - point).GetLengthSquared());
			}
		}

		if (!line.IsHorizontal())
		{
			double y = int(intersect.y + 0.5);
			if (y > bbox.m_p0.y && y < bbox.m_p1.y && !Kernel::fcomp(y, line.GetP0().y) && !Kernel::fcomp(y, line.GetP1().y))
			{
				double dx = line.IsVertical() ? 0 : (y - intersect.y) / line.GetGradient();
				const Jig::Vec2 snapped(intersect.x + dx, y);
				minFinder.Try(std::make_shared<Model::EdgePointElement>(&object, &edge, snapped), Jig::Vec2f(Jig::Vec2f(snapped) - point).GetLengthSquared());
			}
		}
	}
}

void HitTester::HitTestObject(const Model::VectorObject& object, const sf::Vector2f& point, MinFinder& minFinder, float threshold) const
{
	if (m_opts[Option::Verts])
		if (auto* vert = object.GetMesh().FindNearestVert(point, threshold))
			minFinder.Try(std::make_shared<Model::VertElement>(&object, vert), (float)(Jig::Vec2(Jig::Vec2(point) - *vert).GetLengthSquared()));

	if (m_opts[Option::Edges])
		if (auto* face = object.GetMesh().HitTest(Jig::Vec2(point)))
			HitTestEdgeLoop(object, point, face->GetEdges(), minFinder, threshold);
		else
			HitTestEdgeLoop(object, point, object.GetMesh().GetOuterEdges(), minFinder, threshold);
}
