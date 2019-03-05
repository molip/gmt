#include "MergeFace.h"
#include "../Selection.h"
#include "../Model/VectorObject.h"

using namespace GMT::Model;
using namespace GMT::Model::Command;

MergeFace::MergeFace(const Jig::EdgeMesh::Edge& edge, const VectorObject& object) : EdgeMesh(object), m_edge(const_cast<Jig::EdgeMesh::Edge&>(edge))
{
	auto command = std::make_unique<Jig::EdgeMeshCommand::MergeFace>(GetMesh(), m_edge);
	m_command = std::move(command);
}

void MergeFace::Do(CommandContext & ctx)
{
	__super::Do(ctx);

	auto& command = dynamic_cast<Jig::EdgeMeshCommand::MergeFace&>(*m_command);

	for (auto* vert : command.GetDeletedVerts())
		ctx.Deselect(Selection(m_object, *vert));

	m_op = const_cast<VectorObject&>(m_object).GetWallThings().UpdateForMergedFace(command.GetDeletedEdges());
}

void MergeFace::Undo(CommandContext& ctx)
{
	m_op->Undo();
	__super::Undo(ctx);
}

void MergeFace::Redo(CommandContext & ctx)
{
	Do(ctx);
}
