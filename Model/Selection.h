#pragma once

#include "Jig/EdgeMesh.h"

#include <vector>

namespace GMT::Model
{
	namespace Notification
	{
		class Base;
	}

	class Model;
	class VectorObject;

	class Selection
	{
	public:
		Selection() {}
		Selection(const VectorObject& object, const Jig::EdgeMesh::Vert& vert) : m_object(&object), m_vert(&vert) {}

		void Clear() { m_vert = nullptr; }
		bool Contains(const Selection& other) const;
		bool Remove(const Selection& other);

		const Jig::EdgeMesh::Vert* GetVert() const { return m_vert; }
		const VectorObject* GetObject() const { return m_object; }

	private:
		const VectorObject* m_object{};
		const Jig::EdgeMesh::Vert* m_vert{};
	};
}
