#pragma once

#include "EdgeMesh.h"

namespace GMT::Model::Command
{
	class DeleteFace : public EdgeMesh
	{
	public:
		DeleteFace(const Jig::EdgeMesh::Face& face, const VectorObject& object);

		virtual void Do(CommandContext& ctx) override;
		virtual void Undo(CommandContext& ctx) override;
		virtual void Redo(CommandContext& ctx) override;

	private:
		Jig::EdgeMesh::Face& m_face;
		WallThingOpPtr m_op;
	};
}
