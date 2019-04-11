#include "VectorObject.h"
#include "../Grid.h"
#include "../WallMaker.h"

#include "Jig/EdgeMeshInternalEdges.h"
#include "Jig/Geometry.h"
#include "Jig/Mitre.h"
#include "Jig/Polygon.h"
#include "Jig/Triangulator.h"

#include "libKernel/FormatString.h"
#include "libKernel/Log.h"
#include "libKernel/Math.h"
#include "libKernel/MinFinder.h"

#include <SFML/Graphics.hpp>

using namespace GMT;
using namespace GMT::Model;

namespace
{
	const float FloorTextureScale = 0.1f;
}

REGISTER_DYNAMIC(VectorObject)

std::unique_ptr<VectorObject> VectorObject::Create(std::vector<sf::Vector2f> points)
{
	if (points.empty())
		return nullptr;

	// Polygon::Update() doesn't like duplicate adjacent points.
	points.erase(std::unique(points.begin(), points.end()), points.end());

	if (points.size() < 3)
		return nullptr;

	Jig::Polygon poly;
	poly.reserve(points.size());

	for (const auto& point : points)
		poly.push_back({ (double)point.x, (double)point.y });

	poly.Update();
	if (poly.IsSelfIntersecting())
		return nullptr;

	auto object = std::unique_ptr<VectorObject>(new VectorObject(poly));
	return object;
}

VectorObject::VectorObject() : m_wallImage(ImageResource::Type::Wall), m_floorImage(ImageResource::Type::Floor)
{
	m_wallImage.SetID("wall1");
	m_floorImage.SetID("floor1");
	m_pillarImage.SetID("pillar1");
}

VectorObject::VectorObject(const Jig::Polygon& poly) : VectorObject()
{
	std::vector<Jig::EdgeMesh::VertPtr> verts;
	verts.reserve(poly.size());
	for (const auto& point : poly)
		verts.push_back(std::make_unique<Jig::EdgeMesh::Vert>(point));

	m_edgeMesh = std::make_unique<Jig::EdgeMesh>(std::move(verts));

	auto face = std::make_unique<Jig::EdgeMesh::Face>();
	for (const auto& vert : m_edgeMesh->GetVerts())
		face->AddAndConnectEdge(vert.get());

	m_edgeMesh->PushFace(std::move(face));

	Update();
}

VectorObject::~VectorObject() = default;

void VectorObject::Save(Kernel::Serial::SaveNode & node) const
{
	node.SaveClassPtr("mesh", m_edgeMesh);
	node.SaveClass("wall_things", m_wallThings);
	node.SaveClass("wall_image", m_wallImage);
	node.SaveClass("floor_image", m_floorImage);
	node.SaveClass("pillar_image", m_pillarImage);
}

void VectorObject::Load(const Kernel::Serial::LoadNode & node)
{
	node.LoadClassPtr("mesh", m_edgeMesh);
	node.LoadClass("wall_things", m_wallThings);
	node.LoadClass("wall_image", m_wallImage);
	node.LoadClass("floor_image", m_floorImage);
	node.LoadClass("pillar_image", m_pillarImage);
	
	Update();
}

void VectorObject::Draw(RenderContext& rc) const
{
	sf::RenderStates renderStates(rc.GetGrid().GetTransform());

	renderStates.texture = m_floorImage.GetTexture();
	rc.GetWindow().draw(m_floors, renderStates);

	renderStates.texture = m_wallImage.GetTexture();
	rc.GetWindow().draw(m_innerWalls, renderStates);

	renderStates.texture = m_wallImage.GetTexture();
	rc.GetWindow().draw(m_outerWalls, renderStates);

	renderStates.texture = m_pillarImage.GetTexture();
	rc.GetWindow().draw(m_pillars, renderStates);

	renderStates.texture = nullptr;
	m_wallThings.Draw(rc);
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

void VectorObject::Update() const
{
	m_edgeMesh->Update();
	m_edgeMesh->Dump();

	m_floors = GetFloors();

	WallMaker wallMaker(*m_edgeMesh, m_wallImage.GetSize(), m_pillarImage.GetSize());
	m_innerWalls = wallMaker.GetInnerWalls();
	m_outerWalls = wallMaker.GetOuterWalls();
	m_pillars = wallMaker.GetPillars();
}

sf::VertexArray VectorObject::GetFloors() const
{
	sf::VertexArray floors(sf::Triangles);

	const Jig::Vec2f texSize(m_floorImage.GetSize());

	if (texSize.IsZero())
		return {};

	for (auto& face : m_edgeMesh->GetFaces())
	{
		auto poly = face->GetPolygon();
		poly.Update();
		if (!poly.IsSelfIntersecting())
		{
			const auto mesh = MakeTriangleMesh(Jig::Triangulator(poly).Go());
			for (auto& point : *mesh)
				floors.append(sf::Vertex(point, { point.x * texSize.x * FloorTextureScale, point.y * texSize.y * FloorTextureScale }));
		}
	}

	return floors;
}
