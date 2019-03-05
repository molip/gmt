#pragma once

#include "EdgeMesh.h"

namespace GMT::Model::Command
{
	class DeleteVert : public EdgeMesh
	{
	public:
		DeleteVert(const Jig::EdgeMesh::Vert& vert, const VectorObject& object);

		virtual void Do(CommandContext& ctx) override;
		virtual void Undo(CommandContext& ctx) override;
		virtual void Redo(CommandContext& ctx) override;

	private:
		Jig::EdgeMesh::Vert& m_vert;
		WallThingOpPtr m_op;
	};
}
