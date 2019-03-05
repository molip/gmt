#pragma once

#include "WallThing.h"

#include "Jig/EdgeMesh.h"

#include <vector>

namespace GMT::Model
{
	class WallThingOp
	{
	public:
		WallThingOp(WallThings& wallThings) : m_things(wallThings.m_things) {}

		void AddedVert(const Jig::EdgeMesh::Edge& edge);
		void DeletedVert(const std::vector<const Jig::EdgeMesh::Edge*>& edges);
		void MergedFace(const std::vector<const Jig::EdgeMesh::Edge*>& edges);
		void DeletedFace(const Jig::EdgeMesh::Face& face);

		void Undo();
	
	private:
		void AddMove(size_t index, const WallThing::Position& position, bool flip = false);
		void AddFlip(size_t index);
		void AddDelete(size_t index);

		struct Move
		{
			Move(WallThing* thing, const WallThing::Position& position, bool flip) : thing(thing), position(position), flip(flip) {}
			WallThing* thing{};
			WallThing::Position position;
			bool flip;
		};

		struct Delete
		{
			Delete(WallThingPtr thing, size_t index) : thing(std::move(thing)), index(index) {}
			WallThingPtr thing;
			size_t index;
		};

		WallThings::ThingVec& m_things;
		std::vector< std::unique_ptr<Move>> m_moves;
		std::vector< std::unique_ptr<Delete>> m_deletes;
	};
	using WallThingOpPtr = std::unique_ptr<WallThingOp>;
}
