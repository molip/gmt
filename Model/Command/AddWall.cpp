#include "AddWall.h"
#include "../Model.h"

#include "Jig/EdgeMeshCommand.h"
#include "Jig/EdgeMeshAddFace.h"

using namespace GMT::Model;
using namespace GMT::Model::Command;

AddWall::AddWall(const Terminus& start, const Terminus& end, const Jig::PolyLine& points, const VectorObject& object) :
	EdgeMesh(object), m_start(start), m_end(end), m_points(points), m_mesh(const_cast<VectorObject&>(object).GetMesh())
{
}

void AddWall::Do(Model& model)
{
	// We have to add any new verts before calling EdgeMeshAddFace.
	auto[start, end, compound] = GetVerts();
	compound->Do();

	auto& faceCommand = Jig::EdgeMeshAddFace(m_mesh, *start, *end, m_points);
	faceCommand->Do();
	compound->AddChild(std::move(faceCommand));

	m_command = std::move(compound);
	m_object.Update();
}

AddWall::Verts AddWall::GetVerts() const
{
	auto compound = std::make_unique<Jig::EdgeMeshCommand::Compound>();

	auto insertVert = [&](const VectorObject::EdgeTerminus& term)
	{
		auto command = std::make_unique<Jig::EdgeMeshCommand::InsertVert>(m_mesh, const_cast<Jig::EdgeMesh::Edge&>(*term.first), term.second);
		auto* vert = command->GetVert();
		compound->AddChild(std::move(command));
		return vert;
	};

	auto* startEdgeTerm = m_start.GetEdge();
	auto* endEdgeTerm = m_end.GetEdge();

	if (startEdgeTerm && endEdgeTerm && startEdgeTerm->first == endEdgeTerm->first) // Same edge! 
	{
		Jig::EdgeMesh::Vert* startVert{};
		Jig::EdgeMesh::Vert* endVert{};

		// We have to do the one nearest the vert first. 
		std::vector<std::pair<Jig::EdgeMesh::Vert**, const VectorObject::EdgeTerminus*>> vec;
		vec = { { &startVert, startEdgeTerm },{ &endVert, endEdgeTerm } };

		std::sort(vec.begin(), vec.end(), [&](auto& lhs, auto& rhs)
		{
			auto& lhsTerm = *lhs.second;
			auto& rhsTerm = *rhs.second;
			return Jig::Vec2(lhsTerm.second - *lhsTerm.first->vert).GetLengthSquared() < 
				Jig::Vec2(rhsTerm.second - *rhsTerm.first->vert).GetLengthSquared();
		});

		for (auto&[vert, term] : vec)
			*vert = insertVert(*term);

		return { startVert, endVert, std::move(compound) };
	}

	auto getVert = [&](const Terminus& term)
	{
		Jig::EdgeMesh::Vert* result{};

		if (auto* vertTerm = term.GetVert())
			result = const_cast<Jig::EdgeMesh::Vert*>(*vertTerm);
		else if (auto* edgeTerm = term.GetEdge())
			result = insertVert(*edgeTerm);

		return result;
	};

	return { getVert(m_start), getVert(m_end), std::move(compound) };
}

bool GMT::Model::Command::AddWall::CanDo() const
{
	auto[start, end, compound] = GetVerts();
	compound->Do();
	
	bool ok = Jig::EdgeMeshAddFace(m_mesh, *start, *end, m_points) != nullptr;
	compound->Undo();
	return ok;
}