#pragma once

#include "Jig/EdgeMesh.h"

#include <vector>

namespace GMT
{
	class RenderContext;
}

namespace GMT::Model
{
	class WallThingOp;
	using WallThingOpPtr = std::unique_ptr<WallThingOp>;

	class WallThing
	{
	public:
		class Position
		{
		public:
			const Jig::EdgeMesh::Edge* edge{};
			double dist{ 0.5 };
		};

		WallThing(const Jig::EdgeMesh::Edge* edge = nullptr) : m_position{ edge } {}
		void Draw(RenderContext& rc) const;

		const Position& GetPosition() const { return m_position; }
		void SetPosition(const Position& pos) { m_position = pos; }

		void ToggleFlipped() { m_flipped = !m_flipped; }

		void Load(const Kernel::Serial::LoadNode& node);
		void Save(Kernel::Serial::SaveNode& node) const;

	private:
		Position m_position;
		bool m_flipped{};
	};
	using WallThingPtr = std::unique_ptr<WallThing>;

	class WallThings
	{
		friend class WallThingOp;
	public:
		using ThingVec = std::vector<WallThingPtr>;

		void Add(std::unique_ptr<WallThing> thing) { m_things.push_back(std::move(thing)); }
		bool EdgeHasThing(const Jig::EdgeMesh::Edge& edge) const;
		void Draw(RenderContext& rc) const;

		WallThingOpPtr UpdateForAddedVert(const Jig::EdgeMesh::Edge& edge);
		WallThingOpPtr UpdateForDeletedVert(const std::vector<const Jig::EdgeMesh::Edge*>& edges);
		WallThingOpPtr UpdateForMergedFace(const std::vector<const Jig::EdgeMesh::Edge*>& edges);
		WallThingOpPtr UpdateForDeletedFace(const Jig::EdgeMesh::Face& face);

		void Load(const Kernel::Serial::LoadNode& node);
		void Save(Kernel::Serial::SaveNode& node) const;

	private:
		ThingVec m_things;
	};
}
