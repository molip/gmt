#include "VectorObject.h"
#include "../Grid.h"

#include "Jig/EdgeMeshAddFace.h"
#include "Jig/Geometry.h"
#include "Jig/Line2.h"
#include "Jig/Mitre.h"
#include "Jig/Polygon.h"
#include "Jig/Triangulator.h"

#include <SFML/Graphics.hpp>

using namespace GMT;
using namespace GMT::Model;

namespace
{
	const float WallThickness = 1.0f;
	const float FloorTextureScale = 0.1f;
	const Jig::Vec2f WallTextureScale(0.5f, 0.5f);
}

VectorObject::VectorObject(const std::vector<sf::Vector2f>& points)
{
	Init(points);

	m_wallTexture = std::make_unique<sf::Texture>();
	m_wallTexture->loadFromFile("wall.jpg");
	m_wallTexture->setRepeated(true);

	m_floorTexture = std::make_unique<sf::Texture>();
	m_floorTexture->loadFromFile("floor.jpg");
	m_floorTexture->setRepeated(true);

	m_floors.setPrimitiveType(sf::Triangles);
	m_walls.setPrimitiveType(sf::Triangles);

	Update();
}

VectorObject::~VectorObject() = default;

void VectorObject::Draw(RenderContext& rc) const
{
	sf::RenderStates renderStates(rc.GetGrid().GetTransform());

	renderStates.texture = m_floorTexture.get();
	rc.GetWindow().draw(m_floors, renderStates);

	renderStates.texture = m_wallTexture.get();
	rc.GetWindow().draw(m_walls, renderStates);
}

bool VectorObject::AddWall(const Jig::PolyLine& polyline, const Jig::EdgeMesh::Vert& start, const Jig::EdgeMesh::Vert& end)
{
	if (!Jig::EdgeMeshAddFace(*m_edgeMesh, const_cast<Jig::EdgeMesh::Vert&>(start), const_cast<Jig::EdgeMesh::Vert&>(end), polyline))
		return false;

	Update();
	return true;
}

VectorObject::TriangleMeshPtr VectorObject::MakeTriangleMesh(const Jig::EdgeMesh& edgeMesh) const
{
	TriangleMeshPtr mesh = std::make_unique<TriangleMesh>();
	mesh->reserve(edgeMesh.GetFaces().size() * 3);
	for (auto& face : edgeMesh.GetFaces())
		for (auto& point : face->GetPointLoop())
			mesh->push_back({ (float)point.x, (float)point.y });

	return mesh;
}

bool VectorObject::Init(std::vector<sf::Vector2f> points)
{
	if (points.empty())
		return false;

	// Polygon::Update() doesn't like duplicate adjacent points.
	points.erase(std::unique(points.begin(), points.end()), points.end());

	if (points.size() < 3)
		return false;

	Jig::Polygon poly;
	poly.reserve(points.size());

	for (const auto& point : points)
		poly.push_back({ (double)point.x, (double)point.y });

	poly.Update();
	if (poly.IsSelfIntersecting())
		return false;

	std::vector<Jig::EdgeMesh::VertPtr> verts;
	verts.reserve(poly.size());
	for (const auto& point : poly)
		verts.push_back(std::make_unique<Jig::EdgeMesh::Vert>(point));

	m_edgeMesh = std::make_unique<Jig::EdgeMesh>(std::move(verts));
	m_edgeMesh->SetEnableVisiblePoints(false);

	auto face = std::make_unique<Jig::EdgeMesh::Face>();
	for (const auto& vert : m_edgeMesh->GetVerts())
		face->AddAndConnectEdge(vert.get());

	m_edgeMesh->AddFace(std::move(face));

	return true;
}

void VectorObject::Update()
{
	m_edgeMesh->Update();

	UpdateFloors();
	UpdateWalls();
}

void VectorObject::UpdateFloors()
{
	m_floors.clear();

	const Jig::Vec2f texSize(m_floorTexture->getSize());
	m_floors.clear();

	for (auto& face : m_edgeMesh->GetFaces())
	{
		auto poly = face->GetPolygon();
		const auto mesh = MakeTriangleMesh(Jig::Triangulator(poly).Go());
		for (auto& point : *mesh)
			m_floors.append(sf::Vertex(point, { point.x * texSize.x * FloorTextureScale, point.y * texSize.y * FloorTextureScale }));
	}
}

void VectorObject::UpdateWalls()
{
	m_walls.clear();

	UpdateWalls(m_edgeMesh->GetOuterPolygon(), Jig::LineAlignment::Outer);

}

void VectorObject::UpdateWalls(const Jig::PolyLine& polyline, Jig::LineAlignment alignment)
{
	Jig::Polygon inner, outer;
	for (size_t i = 0; i < polyline.size(); ++i)
	{
		Jig::Vec2f prev, next;
		const Jig::Vec2f point = polyline.GetVertex(i);
		
		const Jig::Vec2f *pPrev{}, *pNext{};

		if (polyline.IsValidIndex(i - 1))
		{
			prev = polyline.GetVertex(i - 1);
			pPrev = &prev;
		}

		if (polyline.IsValidIndex(i + 1))
		{
			next = polyline.GetVertex(i + 1);
			pNext = &next;
		}

		auto wallPoints = Jig::GetMitrePoints(point, pPrev, pNext, ::WallThickness, alignment);
		inner.push_back((*wallPoints).first);
		outer.push_back((*wallPoints).second);
	}

	inner.Update();
	outer.Update();

	if (inner.IsSelfIntersecting() || outer.IsSelfIntersecting())
	{
		KERNEL_ASSERT(false);
		return;
	}

	for (size_t i = 0; i < outer.size(); ++i)
	{
		const Jig::Vec2f outer0 = outer.GetVertex(i);
		const Jig::Vec2f inner0 = inner.GetVertex(i);
		const Jig::Vec2f outer1 = outer.GetVertex(i + 1);
		const Jig::Vec2f inner1 = inner.GetVertex(i + 1);
	
		const Jig::Vec2f outerVec(outer1 - outer0);
		const Jig::Vec2f innerVec(inner1 - inner0);
		const Jig::Vec2f edgeVec(inner0 - outer0);

		const float outerLength = outerVec.GetLength();
		const float innerLength = innerVec.GetLength();
		const float edgeLength = edgeVec.GetLength();

		const float cos = outerVec.Normalised().Dot(edgeVec.Normalised());

		const float innerOffset = cos * edgeLength;

		const Jig::Vec2f outerTex0;
		const Jig::Vec2f outerTex1(outerLength, 0);
		const Jig::Vec2f innerTex0(innerOffset, 1);
		const Jig::Vec2f innerTex1(innerOffset + innerLength, 1);

		const Jig::Vec2f texSize(m_wallTexture->getSize());

		auto addPoint = [&](auto& point, auto& tex)
		{
			m_walls.append(sf::Vertex(point, { tex.x * texSize.x * WallTextureScale.x, tex.y * texSize.y * WallTextureScale.y }));
		};

		addPoint(outer0, outerTex0);
		addPoint(outer1, outerTex1);
		addPoint(inner1, innerTex1);
		addPoint(inner1, innerTex1);
		addPoint(inner0, innerTex0);
		addPoint(outer0, outerTex0);
	}
}

const Jig::EdgeMesh::Vert* VectorObject::FindNearestVert(const sf::Vector2f& point, float tolerance) const
{
	return m_edgeMesh->FindNearestVert(point, tolerance);
}

const Jig::EdgeMesh::Face* VectorObject::HitTestRooms(const sf::Vector2f& point) const
{
	for (auto& face : m_edgeMesh->GetFaces())
		if (face->Contains(point))
			return face.get();

	return nullptr;
}
