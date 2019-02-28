#include "MergeFace.h"
#include "../Selection.h"

using namespace GMT::Model;
using namespace GMT::Model::Command;

MergeFace::MergeFace(const Jig::EdgeMesh::Edge& edge, const VectorObject& object) : EdgeMesh(object), m_edge(const_cast<Jig::EdgeMesh::Edge&>(edge))
{
	auto command = std::make_unique<Jig::EdgeMeshCommand::MergeFace>(GetMesh(), m_edge);
	m_command = std::move(command);
}

void GMT::Model::Command::MergeFace::Do(CommandContext & ctx)
{
	__super::Do(ctx);
	
	for (auto* vert : dynamic_cast<Jig::EdgeMeshCommand::MergeFace*>(m_command.get())->GetDeletedVerts())
		ctx.Deselect(Selection(m_object, *vert));
}

void GMT::Model::Command::MergeFace::Redo(CommandContext & ctx)
{
	Do(ctx);
}
