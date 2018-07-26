#include "EdgeMesh.h"
#include "../Model.h"
#include "../Object.h"

#include "Jig/EdgeMeshCommand.h"

using namespace GMT::Model;
using namespace GMT::Model::Command;

EdgeMesh::EdgeMesh(const Object& object) : m_object(object)
{
}

EdgeMesh::EdgeMesh(Ptr command, const Object& object) : m_command(std::move(command)), m_object(object)
{
}

EdgeMesh::EdgeMesh(Jig::EdgeMeshCommand::Compound::Vec&& commands, const Object& object) : m_object(object)
{
	m_command = std::make_unique<Jig::EdgeMeshCommand::Compound>(std::move(commands));
}

void EdgeMesh::Do(CommandContext& ctx)
{
	m_command->Do();
	m_object.Update();
}

void EdgeMesh::Undo(CommandContext& ctx)
{
	m_command->Undo();
	m_object.Update();
}

void EdgeMesh::UndoNoUpdate()
{
	m_command->Undo();
}

void EdgeMesh::Redo(CommandContext& ctx) // In case Do is overridden.
{
	m_command->Do();
	m_object.Update();
}
