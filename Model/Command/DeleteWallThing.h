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
	class DeleteWallThing : public Base
	{
	public:
		DeleteWallThing(const WallThing& wallThing, const VectorObject& object);

		virtual void Do(CommandContext& ctx) override;
		virtual void Undo(CommandContext& ctx) override;

	private:
		VectorObject& m_object;
		const WallThing&  m_wallThing;
		std::pair<WallThingPtr, int> m_undo;
	};
}
