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
		using Verts = std::pair<Jig::EdgeMesh::Vert*, Jig::EdgeMesh::Vert*>;
		using NewVertVec = std::vector<Jig::EdgeMesh::Vert*>;
		using ModifiedEdge = std::pair<const Jig::EdgeMesh::Edge*, NewVertVec>;
		using ModifiedEdges = std::vector<ModifiedEdge>;

		class UpdateWallThingsCommand : public Jig::EdgeMeshCommand::Base
		{
		public:
			UpdateWallThingsCommand(const VectorObject& object, const ModifiedEdges& modifiedEdges) : m_object(object), m_modifiedEdges(modifiedEdges) {}
			virtual void Do() override;
			virtual void Undo() override;
		private:
			const VectorObject& m_object;
			ModifiedEdges m_modifiedEdges;
			std::vector<WallThingOpPtr> m_ops;
		};

		Verts GetVerts(Jig::EdgeMeshCommand::Compound& compound, ModifiedEdges* modifiedEdges) const;

		ElementPtr m_start, m_end;
		Jig::PolyLine m_points;
		NewVertVec m_newVerts;
	};
}
