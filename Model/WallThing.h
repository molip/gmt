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

		Jig::Vec2 GetPoint() const;

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

		ThingVec::const_iterator begin() const { return m_things.begin(); }
		ThingVec::const_iterator end() const { return m_things.end(); }

		WallThing& Insert(WallThingPtr thing, int index = -1);
		std::pair<WallThingPtr, int> Remove(const WallThing& thing);
		WallThingPtr Remove(int index = -1);
		bool EdgeHasThing(const Jig::EdgeMesh::Edge& edge) const;
		void Draw(RenderContext& rc) const;

		WallThingOpPtr UpdateForAddedVert(const Jig::EdgeMesh::Edge& edge, int count = 1);
		WallThingOpPtr UpdateForDeletedVert(const std::vector<const Jig::EdgeMesh::Edge*>& edges);
		WallThingOpPtr UpdateForMergedFace(const std::vector<const Jig::EdgeMesh::Edge*>& edges);
		WallThingOpPtr UpdateForDeletedFace(const Jig::EdgeMesh::Face& face);

		void Load(const Kernel::Serial::LoadNode& node);
		void Save(Kernel::Serial::SaveNode& node) const;

	private:
		ThingVec m_things;
	};
}
