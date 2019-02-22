#pragma once

#include "EdgeMesh.h"

#include "../VectorObject.h"
#include "../Element.h"

#include "Jig/PolyLine.h"

namespace GMT::Model::Command
{
	class AddWall : public EdgeMesh
	{
	public:
		AddWall(ElementPtr start, ElementPtr end, const Jig::PolyLine& points, const VectorObject& object);

		virtual void Do(CommandContext& ctx) override;
		virtual void Undo(CommandContext& ctx) override;
	
		bool CanDo() const;

	private:
		using CompoundPtr = std::unique_ptr<Jig::EdgeMeshCommand::Compound>;
		using Verts = std::tuple<Jig::EdgeMesh::Vert*, Jig::EdgeMesh::Vert*, CompoundPtr>;
		using NewVertVec = std::vector<const Jig::EdgeMesh::Vert*>;

		Verts GetVerts(NewVertVec* newVerts) const;
			
		ElementPtr m_start, m_end;
		Jig::PolyLine m_points;

		NewVertVec m_newVerts;
	};
}
