#pragma once

#include "EdgeMesh.h"

namespace GMT::Model::Command
{
	class AddVert : public EdgeMesh
	{
	public:
		AddVert(const Jig::EdgeMesh::Edge& edge, const Jig::Vec2& position, const VectorObject& object);

		virtual void Do(CommandContext& ctx) override;
		virtual void Undo(CommandContext& ctx) override;
		virtual void Redo(CommandContext& ctx) override { Do(ctx); }

		const Jig::EdgeMesh::Vert* GetNewVert() const { return m_vert; }

	private:
		const Jig::EdgeMesh::Edge& m_edge;
		Jig::EdgeMesh::Vert* m_vert;
		WallThingOpPtr m_op;
	};
}
