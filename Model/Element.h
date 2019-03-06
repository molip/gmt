#pragma once

#include "WallThing.h"

#include "Jig/EdgeMesh.h"

namespace GMT::Model
{
	class VectorObject;

	class Element
	{
	public:
		Jig::Vec2f GetPointF() const { return Jig::Vec2f(GetPoint()); }
		virtual Jig::Vec2 GetPoint() const { KERNEL_FAIL; return {}; }
		virtual const VectorObject* GetObject() const { return nullptr; }

		template <typename T> 
		const T* GetAs() const { return dynamic_cast<const T*>(this); }
	};
	using ElementPtr = std::shared_ptr<Element>;

	class VertElement : public Element
	{
	public:
		VertElement(const VectorObject* object, const Jig::EdgeMesh::Vert* vert) : object(object), vert(vert) {}
		virtual Jig::Vec2 GetPoint()  const override { return *vert; }
		virtual const VectorObject* GetObject()  const override { return object; }
		const VectorObject* object{};
		const Jig::EdgeMesh::Vert* vert{};
	};
	using VertElementPtr = std::shared_ptr<VertElement>;

	class EdgeElement : public Element
	{
	public:
		EdgeElement(const VectorObject* object, const Jig::EdgeMesh::Edge* edge, double normalisedDist) : object(object), edge(edge), normalisedDist(normalisedDist) {}
		EdgeElement(const VectorObject* object, const Jig::EdgeMesh::Edge* edge, const Jig::Vec2& point) : object(object), edge(edge), normalisedDist(Jig::Vec2(point - *edge->vert).GetLength() / edge->GetVec().GetLength()) {}
		virtual const VectorObject* GetObject() const override { return object; }
		virtual Jig::Vec2 GetPoint() const override { return *edge->vert + edge->GetVec() * normalisedDist; }
		const VectorObject* object{};
		const Jig::EdgeMesh::Edge* edge{};
		const double normalisedDist;
	};
	using EdgeElementPtr = std::shared_ptr<EdgeElement>;

	class GridElement : public Element
	{
	public:
		GridElement(const Jig::EdgeMesh::Face* face, const Jig::Vec2& point) : face(face), point(point) {}
		virtual Jig::Vec2 GetPoint() const { return point; }
		const Jig::EdgeMesh::Face* face{};
		const Jig::Vec2 point;
	};

	class FaceElement : public Element
	{
	public:
		FaceElement(const VectorObject* object, const Jig::EdgeMesh::Face* face) : object(object), face(face) {}
		virtual const VectorObject* GetObject() const override { return object; }
		const VectorObject* object{};
		const Jig::EdgeMesh::Face* face{};
	};

	class WallThingElement : public Element
	{
	public:
		WallThingElement(const VectorObject* object, const WallThing* thing) : object(object), thing(thing) {}
		virtual const VectorObject* GetObject() const override { return object; }
		virtual Jig::Vec2 GetPoint() const { return thing->GetPoint(); }
		const VectorObject* object{};
		const WallThing* thing{};
	};
}
