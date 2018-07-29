#include "DeleteVert.h"
#include "../Selection.h"

using namespace GMT::Model;
using namespace GMT::Model::Command;

DeleteVert::DeleteVert(const Jig::EdgeMesh::Vert& vert, const VectorObject& object) : EdgeMesh(object), m_vert(const_cast<Jig::EdgeMesh::Vert&>(vert))
{
	auto command = std::make_unique<Jig::EdgeMeshCommand::DeleteVert>(GetMesh(), m_vert);
	m_command = std::move(command);
}

void GMT::Model::Command::DeleteVert::Do(CommandContext & ctx)
{
	__super::Do(ctx);
	ctx.Deselect(Selection(m_object, m_vert));
}

void GMT::Model::Command::DeleteVert::Redo(CommandContext & ctx)
{
	Do(ctx);
}
