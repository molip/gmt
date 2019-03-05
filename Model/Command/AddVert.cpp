#include "AddVert.h"
#include "../Selection.h"
#include "../Model/VectorObject.h"

using namespace GMT::Model;
using namespace GMT::Model::Command;

AddVert::AddVert(const Jig::EdgeMesh::Edge& edge, const Jig::Vec2& position, const VectorObject& object) : EdgeMesh(object), m_edge(edge)
{
	auto command = std::make_unique<Jig::EdgeMeshCommand::InsertVerts>(GetMesh(), const_cast<Jig::EdgeMesh::Edge&>(edge), position);
	m_vert = command->GetVerts().front().get();
	m_command = std::move(command);
}

void GMT::Model::Command::AddVert::Do(CommandContext & ctx)
{
	__super::Do(ctx);
	m_op = const_cast<VectorObject&>(m_object).GetWallThings().UpdateForAddedVert(m_edge);
}

void GMT::Model::Command::AddVert::Undo(CommandContext & ctx)
{
	m_op->Undo();
	__super::Undo(ctx);
	ctx.Deselect(Selection(m_object, *m_vert));
}
