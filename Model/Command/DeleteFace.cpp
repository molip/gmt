#include "DeleteFace.h"
#include "../Selection.h"

using namespace GMT::Model;
using namespace GMT::Model::Command;

DeleteFace::DeleteFace(const Jig::EdgeMesh::Face& face, const VectorObject& object) : EdgeMesh(object), m_face(const_cast<Jig::EdgeMesh::Face&>(face))
{
	auto command = std::make_unique<Jig::EdgeMeshCommand::DeleteFace>(GetMesh(), m_face);
	m_command = std::move(command);
}

void GMT::Model::Command::DeleteFace::Do(CommandContext & ctx)
{
	__super::Do(ctx);
	
	for (auto* vert : dynamic_cast<Jig::EdgeMeshCommand::DeleteFace*>(m_command.get())->GetDeletedVerts())
		ctx.Deselect(Selection(m_object, *vert));
}

void GMT::Model::Command::DeleteFace::Redo(CommandContext & ctx)
{
	Do(ctx);
}
