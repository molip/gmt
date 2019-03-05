#include "DeleteFace.h"
#include "../Selection.h"
#include "../Model/VectorObject.h"

using namespace GMT::Model;
using namespace GMT::Model::Command;

DeleteFace::DeleteFace(const Jig::EdgeMesh::Face& face, const VectorObject& object) : EdgeMesh(object), m_face(const_cast<Jig::EdgeMesh::Face&>(face))
{
	auto command = std::make_unique<Jig::EdgeMeshCommand::DeleteFace>(GetMesh(), m_face);
	m_command = std::move(command);
}

void GMT::Model::Command::DeleteFace::Do(CommandContext& ctx)
{
	__super::Do(ctx);

	auto& command = dynamic_cast<Jig::EdgeMeshCommand::DeleteFace&>(*m_command);

	for (auto* vert : command.GetDeletedVerts())
		ctx.Deselect(Selection(m_object, *vert));

	m_op = const_cast<VectorObject&>(m_object).GetWallThings().UpdateForDeletedFace(m_face);
}

void DeleteFace::Undo(CommandContext& ctx)
{
	__super::Undo(ctx);
	m_op->Undo();
}

void DeleteFace::Redo(CommandContext & ctx)
{
	Do(ctx);
}
