#pragma once

#include "ImageResource.h"

#include "Jig/EdgeMesh.h"

#include <vector>

namespace sf
{
	class Texture;
}

namespace GMT
{
	class RenderContext;
}

namespace GMT::Model
{
	class ImageResource;
	class WallThingOp;
	using WallThingOpPtr = std::unique_ptr<WallThingOp>;

	class WallThing : public Kernel::Dynamic
	{
	public:
		class Position
		{
		public:
			const Jig::EdgeMesh::Edge* edge{};
			double dist{ 0.5 };
		};

		WallThing();

		virtual void Draw(RenderContext& rc) const = 0;
		virtual bool IsSurface() const { return true; }

		const Position& GetPosition() const { return m_position; }
		void SetPosition(const Position& pos) { m_position = pos; }

		Jig::Vec2 GetPoint() const;

		bool CanFlip() const { return !IsSurface(); }
		void ToggleFlipped() { m_flipped = !m_flipped; }

		void SetImageID(const std::string& id) { m_image.SetID(id); }

		void Load(const Kernel::Serial::LoadNode& node);
		void Save(Kernel::Serial::SaveNode& node) const;

	protected:
		Position m_position;
		bool m_flipped{};
		ObjectImageResourceRef m_image;
	};
	using WallThingPtr = std::unique_ptr<WallThing>;

	class Door : public WallThing
	{
	public:
		Door();
		virtual void Draw(RenderContext& rc) const override;
		virtual bool IsSurface() const { return false; }
	};

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
