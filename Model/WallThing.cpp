#include "WallThing.h"
#include "WallThingOp.h"

#include "../RenderContext.h"
#include "../Grid.h"

#include <SFML/Graphics.hpp>

using namespace GMT;
using namespace GMT::Model;

void WallThing::Draw(RenderContext& rc) const
{
	Jig::Vec2 v = m_position.edge->GetVec();
	Jig::Vec2 p = *m_position.edge->vert + v * m_position.dist + Jig::Vec2(-v.y, v.x).Normalised() * 0.2;

	sf::CircleShape circle(10);
	circle.setFillColor(sf::Color::Magenta);
	circle.setPosition(rc.GetGrid().GetPoint(sf::Vector2f(p)) - sf::Vector2f(10, 10));
	rc.GetWindow().draw(circle);
}

void WallThing::Load(const Kernel::Serial::LoadNode& node)
{
	node.LoadObjectRef("edge", m_position.edge);
	node.LoadType("dist", m_position.dist);
	node.LoadType("flipped", m_flipped);
}

void WallThing::Save(Kernel::Serial::SaveNode& node) const
{
	node.SaveObjectRef("edge", m_position.edge);
	node.SaveType("dist", m_position.dist);
	node.SaveType("flipped", m_flipped);
}



bool WallThings::EdgeHasThing(const Jig::EdgeMesh::Edge& edge) const
{
	auto f = [&](auto& item) { return item->GetPosition().edge == &edge; };
	return std::find_if(m_things.begin(), m_things.end(), f) != m_things.end();
}

void WallThings::Draw(RenderContext& rc) const
{
	for (auto& thing : m_things)
		thing->Draw(rc);
}

WallThingOpPtr WallThings::UpdateForAddedVert(const Jig::EdgeMesh::Edge& edge)
{
	WallThingOpPtr op = std::make_unique<WallThingOp>(*this);
	op->AddedVert(edge);
	return op;
}

WallThingOpPtr WallThings::UpdateForDeletedVert(const std::vector<const Jig::EdgeMesh::Edge*>& edges)
{
	WallThingOpPtr op = std::make_unique<WallThingOp>(*this);
	op->DeletedVert(edges);
	return op;
}

WallThingOpPtr WallThings::UpdateForMergedFace(const std::vector<const Jig::EdgeMesh::Edge*>& edges)
{
	WallThingOpPtr op = std::make_unique<WallThingOp>(*this);
	op->MergedFace(edges);
	return op;
}

WallThingOpPtr WallThings::UpdateForDeletedFace(const Jig::EdgeMesh::Face& face)
{
	WallThingOpPtr op = std::make_unique<WallThingOp>(*this);
	op->DeletedFace(face);
	return op;
}

void WallThings::Load(const Kernel::Serial::LoadNode& node)
{
	node.LoadCntr("things", m_things, Kernel::Serial::ClassPtrLoader());
}

void WallThings::Save(Kernel::Serial::SaveNode& node) const
{
	node.SaveCntr("things", m_things, Kernel::Serial::ClassPtrSaver());
}
