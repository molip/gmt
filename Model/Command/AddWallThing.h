#pragma once

#include "Base.h"

namespace GMT::Model
{
	class VectorObject;
	class WallThing;
	using WallThingPtr = std::unique_ptr<WallThing>;
}

namespace GMT::Model::Command
{
	class AddWallThing : public Base
	{
	public:
		AddWallThing(const VectorObject& object, WallThingPtr wallThing);

		virtual void Do(CommandContext& ctx) override;
		virtual void Undo(CommandContext& ctx) override;

	private:
		VectorObject& m_object;
		WallThingPtr m_wallThing;
	};
}
