#pragma once

#include "Object.h"
#include "ImageResource.h"
#include "WallThing.h"

#include "Jig/EdgeMesh.h"
#include "Jig/Line2.h"
#include "Jig/VectorFwd.h"

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

#include <optional>
#include <variant>
#include <vector>

namespace Jig
{
	class PolyLine;
	enum class LineAlignment;
}

namespace sf
{
	class Texture;
}

namespace GMT::Model
{
	class VectorObject : public Object
	{
	public:
		static std::unique_ptr<VectorObject> Create(std::vector<sf::Vector2f> points);
			
		VectorObject();
		virtual ~VectorObject();

		virtual void Save(Kernel::Serial::SaveNode& node) const;
		virtual void Load(const Kernel::Serial::LoadNode& node);

		virtual void Draw(RenderContext& rc) const override;
		virtual void Update() const override;

		Jig::EdgeMesh& GetMesh() { return *m_edgeMesh; }
		const Jig::EdgeMesh& GetMesh() const { return *m_edgeMesh; }

		const WallThings& GetWallThings() const { return m_wallThings; }
		WallThings& GetWallThings() { return m_wallThings; }

		void SetWallImageID(const std::string& id) { m_wallImage.SetID(id); }
		void SetFloorImageID(const std::string& id) { m_floorImage.SetID(id); }
		void SetPillarImageID(const std::string& id) { m_pillarImage.SetID(id); }

	private:
		VectorObject(const Jig::Polygon& poly);

		using WallPoints = std::optional<std::pair<Jig::Vec2f, Jig::Vec2f>>;
		using TriangleMesh = std::vector<sf::Vector2f>;
		using TriangleMeshPtr = std::unique_ptr<TriangleMesh>;

		sf::VertexArray GetFloors() const;

		TriangleMeshPtr MakeTriangleMesh(const Jig::EdgeMesh& edgeMesh) const;

		mutable sf::VertexArray m_floors, m_innerWalls, m_outerWalls, m_pillars;

		std::unique_ptr<Jig::EdgeMesh> m_edgeMesh;
		ObjectImageResourceRef m_pillarImage;
		ImageResourceRef m_wallImage, m_floorImage;

		WallThings m_wallThings;
	};
}
