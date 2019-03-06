#pragma once

#include "Base.h"
#include "../WallThing.h"

namespace GMT::Model
{
	class VectorObject;
}

namespace GMT::Model::Command
{
	class MoveWallThing : public Base
	{
	public:
		MoveWallThing(const WallThing& wallThing, const WallThing::Position& position);

		virtual void Do(CommandContext& ctx) override;
		virtual void Undo(CommandContext& ctx) override;
		virtual void UndoNoUpdate() override;

	private:
		WallThing& m_wallThing;
		WallThing::Position m_position;
	};
}
