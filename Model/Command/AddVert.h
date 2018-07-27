#pragma once

#include "EdgeMesh.h"

namespace GMT::Model::Command
{
	class AddVert : public EdgeMesh
	{
	public:
		AddVert(const Jig::EdgeMesh::Edge& edge, const Jig::Vec2& position, const VectorObject& object);

		virtual void Undo(CommandContext& ctx) override;
	private:
		Jig::EdgeMesh::Vert* m_vert;
	};
}
