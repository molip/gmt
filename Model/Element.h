#pragma once

#include "Jig/EdgeMesh.h"

namespace GMT::Model
{
	class VectorObject;

	class Element
	{
	public:
		virtual Jig::Vec2f GetPoint() const { KERNEL_FAIL; return {}; }
		virtual const VectorObject* GetObject() const { return nullptr; }

		template <typename T> 
		const T* GetAs() const { return dynamic_cast<const T*>(this); }
	};
	using ElementPtr = std::shared_ptr<Element>;

	class VertElement : public Element
	{
	public:
		VertElement(const VectorObject* object, const Jig::EdgeMesh::Vert* vert) : object(object), vert(vert) {}
		virtual Jig::Vec2f GetPoint()  const override { return Jig::Vec2f(*vert); }
		virtual const VectorObject* GetObject()  const override { return object; }
		const VectorObject* object{};
		const Jig::EdgeMesh::Vert* vert{};
	};
	using VertElementPtr = std::shared_ptr<VertElement>;

	class EdgeElement : public Element
	{
	public:
		EdgeElement(const VectorObject* object, const Jig::EdgeMesh::Edge* edge) : object(object), edge(edge) {}
		virtual const VectorObject* GetObject() const override { return object; }
		const VectorObject* object{};
		const Jig::EdgeMesh::Edge* edge{};
	};
	using EdgeElementPtr = std::shared_ptr<EdgeElement>;

	class EdgePointElement : public EdgeElement
	{
	public:
		EdgePointElement(const VectorObject* object, const Jig::EdgeMesh::Edge* edge, const Jig::Vec2& point) : EdgeElement(object, edge), point(point) {}
		virtual Jig::Vec2f GetPoint() const override { return Jig::Vec2f(point); }
		const Jig::Vec2 point;
	};
	using EdgePointElementPtr = std::shared_ptr<EdgePointElement>;

	class GridElement : public Element
	{
	public:
		GridElement(const Jig::EdgeMesh::Face* face, const Jig::Vec2& point) : face(face), point(point) {}
		virtual Jig::Vec2f GetPoint() const { return Jig::Vec2f(point); }
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
}
