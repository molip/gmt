#include "AddWall.h"
#include "../Model.h"
#include "../Selection.h"

#include "Jig/EdgeMeshCommand.h"
#include "Jig/EdgeMeshAddFace.h"

using namespace GMT::Model;
using namespace GMT::Model::Command;

AddWall::AddWall(ElementPtr start, ElementPtr end, const Jig::PolyLine& points, const VectorObject& object) :
	EdgeMesh(object), m_start(std::move(start)), m_end(std::move(end)), m_points(points)
{
}

void AddWall::Do(CommandContext& ctx)
{
	// We have to add any new verts before calling EdgeMeshAddFace.
	auto[start, end, compound] = GetVerts(&m_newVerts);
	compound->Do();

	auto& faceCommand = Jig::EdgeMeshAddFace(GetMesh(), *start, *end, m_points);

	for (auto& vert : faceCommand->GetNewVerts())
		m_newVerts.push_back(vert.get());

	faceCommand->Do();

	compound->AddChild(std::move(faceCommand));

	m_command = std::move(compound);
	m_object.Update();
}

void GMT::Model::Command::AddWall::Undo(CommandContext & ctx)
{
	__super::Undo(ctx);
	
	for (auto& vert : m_newVerts)
		ctx.Deselect(Selection(m_object, *vert));
}

AddWall::Verts AddWall::GetVerts(NewVertVec* newVerts) const
{
	auto compound = std::make_unique<Jig::EdgeMeshCommand::Compound>();

	auto insertVerts = [&](const Jig::EdgeMesh::Edge& edge, std::initializer_list<Jig::Vec2> positions)
	{
		auto command = std::make_unique<Jig::EdgeMeshCommand::InsertVerts>(GetMesh(), const_cast<Jig::EdgeMesh::Edge&>(edge), positions);
		std::vector<Jig::EdgeMesh::Vert*> verts;
		for (auto& vert : command->GetVerts())
			verts.push_back(vert.get());

		if (newVerts)
			newVerts->insert(newVerts->end(), verts.begin(), verts.end());

		compound->AddChild(std::move(command));

		return verts;
	};

	auto* startEdgeTerm = m_start->GetAs<EdgePointElement>();
	auto* endEdgeTerm = m_end->GetAs<EdgePointElement>();

	if (startEdgeTerm && endEdgeTerm && startEdgeTerm->edge == endEdgeTerm->edge || startEdgeTerm->edge == endEdgeTerm->edge->twin) // Same edge! 
	{
		const auto& edge = *startEdgeTerm->edge;

		// Sort by position along edge.
		const bool swap = Jig::Vec2(endEdgeTerm->point - *edge.vert).GetLengthSquared() <
			Jig::Vec2(startEdgeTerm->point - *edge.vert).GetLengthSquared();

		std::vector<const EdgePointElement*> terms{ startEdgeTerm, endEdgeTerm };
		auto verts = insertVerts(edge, { terms[swap]->point, terms[!swap]->point });
		return { verts[swap], verts[!swap], std::move(compound) };
	}

	auto getVert = [&](const Element& term)
	{
		Jig::EdgeMesh::Vert* result{};

		if (auto* vertTerm = term.GetAs<VertElement>())
			result = const_cast<Jig::EdgeMesh::Vert*>(vertTerm->vert);
		else if (auto* edgeTerm = term.GetAs<EdgePointElement>())
			result = insertVerts(*edgeTerm->edge, { edgeTerm->point }).front();

		return result;
	};

	return { getVert(*m_start), getVert(*m_end), std::move(compound) };
}

bool GMT::Model::Command::AddWall::CanDo() const
{
	auto[start, end, compound] = GetVerts(nullptr);
	compound->Do();
	
	bool ok = Jig::EdgeMeshAddFace(GetMesh(), *start, *end, m_points) != nullptr;
	compound->Undo();
	return ok;
}
