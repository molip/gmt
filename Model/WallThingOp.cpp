#include "WallThingOp.h"

using namespace GMT;
using namespace GMT::Model;

void WallThingOp::AddMove(size_t index, const WallThing::Position& position, bool flip)
{
	auto& thing = m_things[index];
	m_moves.push_back(std::make_unique<Move>(thing.get(), thing->GetPosition(), flip));
	thing->SetPosition(position);
	if (flip)
		thing->ToggleFlipped();
}

void WallThingOp::AddFlip(size_t index)
{
	const auto& pos = m_things[index]->GetPosition();
	AddMove(index, WallThing::Position{ pos.edge->twin, 1 - pos.dist }, true);
}

void WallThingOp::AddDelete(size_t index)
{
	m_deletes.emplace_back(std::make_unique<Delete>(std::move(m_things[index]), index));
	m_things.erase(m_things.begin() + index);
}

void WallThingOp::Undo()
{
	for (auto& op : Kernel::Reverse(m_deletes))
		m_things.insert(m_things.begin() + op->index, std::move(op->thing));

	for (auto& op : m_moves)
	{
		op->thing->SetPosition(op->position);
		if (op->flip)
			op->thing->ToggleFlipped();
	}
}

void WallThingOp::AddedVert(const Jig::EdgeMesh::Edge& edge)
{
	auto f = [&](int index, double vertDist)
	{
		auto& thing = m_things[index];

		double newDist{};
		double oldDist = thing->GetPosition().dist;
		auto* newEdge = thing->GetPosition().edge;


		if (oldDist <= vertDist)
		{
			newDist = oldDist / vertDist;
		}
		else
		{
			newDist = (oldDist - vertDist) / (1 - vertDist);
			newEdge = newEdge->next;
		}

		AddMove(index, WallThing::Position{ newEdge, newDist });
	};

	const double oldLength = Jig::Vec2(*edge.next->next->vert - *edge.vert).GetLength();
	const double newLength = Jig::Vec2(*edge.next->vert - *edge.vert).GetLength();
	const double newVertDist = newLength / oldLength;

	for (int i = 0; i < m_things.size(); ++i)
	{
		if (m_things[i]->GetPosition().edge == &edge)
			f(i, newVertDist);

		if (m_things[i]->GetPosition().edge == edge.next->twin) // Thing on edge's old twin.
			f(i, 1 - newVertDist); 
	}
}

void WallThingOp::DeletedVert(const std::vector<const Jig::EdgeMesh::Edge*>& edges)
{
	for (auto& edge : edges)
	{
		const double oldLength = Jig::Vec2(*edge->prev->vert - *edge->vert).GetLength();
		const double newLength = Jig::Vec2(*edge->next->vert - *edge->prev->vert).GetLength();
		const double oldVertDist = oldLength / newLength;

		for (int i = 0; i < m_things.size(); ++i)
		{
			auto& thingEdge = m_things[i]->GetPosition().edge;

			if (thingEdge != edge && thingEdge != edge->prev)
				continue;

			double newDist{};
			double oldDist = m_things[i]->GetPosition().dist;
			auto* newEdge = m_things[i]->GetPosition().edge;

			if (thingEdge == edge)
			{
				newDist = oldDist + (1 - oldDist) * oldVertDist;
				newEdge = edge->prev;
			}
			else
			{
				newDist = oldDist * oldVertDist;
			}

			AddMove(i, WallThing::Position{ newEdge, newDist });
		}
	}
}

void WallThingOp::MergedFace(const std::vector<const Jig::EdgeMesh::Edge*>& edges)
{
	for (auto& edge : edges)
		for (int i = int(m_things.size()) - 1; i >= 0; --i)
			if (m_things[i]->GetPosition().edge == edge)
				AddDelete(i);
}

void WallThingOp::DeletedFace(const Jig::EdgeMesh::Face& face)
{
	for (auto& edge : face.GetEdges())
		for (int i = int(m_things.size()) - 1; i >= 0; --i)
			if (m_things[i]->GetPosition().edge == &edge)
				if (edge.twin)
					AddFlip(i);
				else
					AddDelete(i);
}
