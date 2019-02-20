#pragma once

#include "Object.h"

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

		using VertTerminus = const Jig::EdgeMesh::Vert*;
		using EdgeTerminus = std::pair<const Jig::EdgeMesh::Edge*, Jig::Vec2>;
		struct Terminus : public std::variant<std::monostate, VertTerminus, EdgeTerminus>
		{
			using std::variant<std::monostate, VertTerminus, EdgeTerminus>::variant;
			const VertTerminus* GetVert() const { return std::get_if<VertTerminus>(this); }
			const EdgeTerminus* GetEdge() const { return std::get_if<EdgeTerminus>(this); }
			bool IsEmpty() const { return index() == 0; }
			const Jig::Vec2* GetPoint() const
			{
				if (auto vert = GetVert())
					return *vert;
				if (auto edge = GetEdge())
					return &edge->second;
				return nullptr;
			}

			void Log(int indent) const;
		};

		Terminus HitTestEdges(const sf::Vector2f& point, float tolerance, float& distSquared) const;
		const Jig::EdgeMesh::Face* HitTestRooms(const sf::Vector2f& point) const;

		Jig::EdgeMesh& GetMesh() { return *m_edgeMesh; }
		const Jig::EdgeMesh& GetMesh() const { return *m_edgeMesh; }

	private:
		VectorObject(const Jig::Polygon& poly);

		using WallPoints = std::optional<std::pair<Jig::Vec2f, Jig::Vec2f>>;
		using TriangleMesh = std::vector<sf::Vector2f>;
		using TriangleMeshPtr = std::unique_ptr<TriangleMesh>;

		sf::VertexArray GetFloors() const;

		TriangleMeshPtr MakeTriangleMesh(const Jig::EdgeMesh& edgeMesh) const;

		mutable sf::VertexArray m_floors, m_innerWalls, m_outerWalls, m_pillars;

		std::unique_ptr<Jig::EdgeMesh> m_edgeMesh;
		std::unique_ptr<sf::Texture> m_wallTexture, m_floorTexture, m_pillarTexture;
	};
}
