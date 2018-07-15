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

	auto getVert = [&](const Terminus& term)
	{
		const Jig::EdgeMesh::Vert* result{};

		if (auto* vertTerm = term.GetVert())
		{
			result = *vertTerm;
		}
		else if (auto* edgeTerm = term.GetEdge())
		{
			auto& edge = *const_cast<Jig::EdgeMesh::Edge*>(edgeTerm->first);
			auto command = std::make_unique<Jig::EdgeMeshCommand::InsertVert>(m_mesh, edge, edgeTerm->second);
			result = command->GetVert();
			compound->AddChild(std::move(command));
		}

		return const_cast<Jig::EdgeMesh::Vert*>(result);
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
