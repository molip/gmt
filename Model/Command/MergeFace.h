#pragma once

#include "EdgeMesh.h"

namespace GMT::Model::Command
{
	class MergeFace : public EdgeMesh
	{
	public:
		MergeFace(const Jig::EdgeMesh::Edge& edge, const VectorObject& object);

		virtual void Do(CommandContext& ctx) override;
		virtual void Redo(CommandContext& ctx) override;

	private:
		Jig::EdgeMesh::Edge& m_edge;
	};
}
