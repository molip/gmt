#include "AddVert.h"
#include "../Selection.h"

using namespace GMT::Model;
using namespace GMT::Model::Command;

AddVert::AddVert(const Jig::EdgeMesh::Edge& edge, const Jig::Vec2& position, const VectorObject& object) : EdgeMesh(object)
{
	auto command = std::make_unique<Jig::EdgeMeshCommand::InsertVert>(GetMesh(), const_cast<Jig::EdgeMesh::Edge&>(edge), position);
	m_vert = command->GetVert();
	m_command = std::move(command);
}

void GMT::Model::Command::AddVert::Undo(CommandContext & ctx)
{
	__super::Undo(ctx);
	ctx.Deselect(Selection(m_object, *m_vert));
}
