#include "MoveWallThing.h"

using namespace GMT::Model;
using namespace GMT::Model::Command;

MoveWallThing::MoveWallThing(const WallThing& wallThing, const WallThing::Position& position) : m_wallThing(const_cast<WallThing&>(wallThing)), m_position(position)
{
}

void MoveWallThing::Do(CommandContext& ctx)
{
	UndoNoUpdate();
}

void MoveWallThing::Undo(CommandContext& ctx)
{
	UndoNoUpdate();
}

void MoveWallThing::UndoNoUpdate()
{
	WallThing::Position position = m_wallThing.GetPosition();
	m_wallThing.SetPosition(m_position);
	m_position = position;
}
