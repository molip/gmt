#pragma once

#include "EdgeMesh.h"

#include "../VectorObject.h"

#include "Jig/PolyLine.h"

namespace GMT::Model::Command
{
	class AddWall : public EdgeMesh
	{
		using Terminus = VectorObject::Terminus;
	public:
		AddWall(const Terminus& start, const Terminus& end, const Jig::PolyLine& points, const VectorObject& object);

		virtual void Do(Model& model) override;
	
		bool CanDo() const;

	private:
		using CompoundPtr = std::unique_ptr<Jig::EdgeMeshCommand::Compound>;
		using Verts = std::tuple<Jig::EdgeMesh::Vert*, Jig::EdgeMesh::Vert*, CompoundPtr>;
		Verts GetVerts() const;
			
		const Terminus m_start, m_end;
		Jig::PolyLine m_points;
		Jig::EdgeMesh& m_mesh;
	};
}
