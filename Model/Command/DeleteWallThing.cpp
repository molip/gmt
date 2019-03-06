#include "DeleteWallThing.h"
#include "../Model.h"
#include "../Selection.h"
#include "../VectorObject.h"

using namespace GMT::Model;
using namespace GMT::Model::Command;

DeleteWallThing::DeleteWallThing(const WallThing& wallThing, const VectorObject& object) : m_wallThing(wallThing), m_object(const_cast<VectorObject&>(object))
{
}

void DeleteWallThing::Do(CommandContext& ctx)
{
	m_undo = m_object.GetWallThings().Remove(std::move(m_wallThing));
}

void DeleteWallThing::Undo(CommandContext& ctx)
{
	m_object.GetWallThings().Insert(std::move(m_undo.first), m_undo.second);
}
