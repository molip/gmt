#include "WallThing.h"
#include "WallThingOp.h"

#include "../RenderContext.h"
#include "../Grid.h"

using namespace GMT;
using namespace GMT::Model;

WallThing::WallThing()
{
}

Jig::Vec2 WallThing::GetPoint() const
{
	const Jig::Vec2 v = m_position.edge->GetVec();
	return *m_position.edge->vert + v * m_position.dist;
}

void WallThing::Load(const Kernel::Serial::LoadNode& node)
{
	node.LoadObjectRef("edge", m_position.edge);
	node.LoadType("dist", m_position.dist);
	node.LoadType("flipped", m_flipped);
	node.LoadClass("image", m_image);
}

void WallThing::Save(Kernel::Serial::SaveNode& node) const
{
	node.SaveObjectRef("edge", m_position.edge);
	node.SaveType("dist", m_position.dist);
	node.SaveType("flipped", m_flipped);
	node.SaveClass("image", m_image);
}


REGISTER_DYNAMIC(Door)

Door::Door()
{
	SetImageID("door1");
}

void Door::Draw(RenderContext& rc) const
{
	const auto centre = rc.GetGrid().GetPoint(sf::Vector2f(GetPoint()));
	auto offset = -Jig::Vec2f(m_image.GetSize()) * 0.5f;
	if (!m_position.edge->twin)
		offset.y *= 2;

	sf::Transform xf;
	xf.translate(centre);
	xf.rotate(-float(m_position.edge->GetVec().Normalised().GetAngle(Jig::Vec2(1, 0)) * 180 / 3.14159));
	xf.translate(offset);

	sf::RenderStates renderStates(xf);
	m_image.Draw(rc, renderStates);
}



WallThing& WallThings::Insert(WallThingPtr thing, int index)
{
	m_things.insert(index >= 0 ? m_things.begin() + index : m_things.end(), std::move(thing));
	return *m_things.back();
}

WallThingPtr WallThings::Remove(int index)
{
	auto it = index >= 0 ? m_things.begin() + index : m_things.end() - 1;
	WallThingPtr thing = std::move(*it);
	m_things.erase(it);
	return thing;
}

std::pair<WallThingPtr, int> WallThings::Remove(const WallThing& thing)
{
	auto it = std::find_if(m_things.begin(), m_things.end(), [&](auto& item) { return item.get() == &thing; });
	if (it == m_things.end())
		return { nullptr, -1 };
	
	const int index = int(it - m_things.begin());
	return { Remove(index), index };
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

WallThingOpPtr WallThings::UpdateForAddedVert(const Jig::EdgeMesh::Edge& edge, int count)
{
	WallThingOpPtr op = std::make_unique<WallThingOp>(*this);
	op->AddedVert(edge, count);
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
	node.LoadCntr("things", m_things, Kernel::Serial::ObjectLoader());
}

void WallThings::Save(Kernel::Serial::SaveNode& node) const
{
	node.SaveCntr("things", m_things, Kernel::Serial::ObjectSaver());
}
