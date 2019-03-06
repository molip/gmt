#include "AddWallThing.h"
#include "../Model.h"
#include "../Selection.h"
#include "../VectorObject.h"

using namespace GMT::Model;
using namespace GMT::Model::Command;

AddWallThing::AddWallThing(const VectorObject& object, WallThingPtr wallThing) : m_object(const_cast<VectorObject&>(object)), m_wallThing(std::move(wallThing))
{
}

void AddWallThing::Do(CommandContext& ctx)
{
	m_object.GetWallThings().Insert(std::move(m_wallThing));
}

void AddWallThing::Undo(CommandContext& ctx)
{
	m_wallThing = m_object.GetWallThings().Remove();
}
