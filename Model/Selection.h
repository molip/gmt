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
	class Object;

	class Selection
	{
	public:
		Selection() {}
		Selection(const Jig::EdgeMesh::Vert& vert) : m_vert(&vert) {}

		void Clear() { m_vert = nullptr; }
		bool Contains(const Selection& other) const;
		bool Remove(const Selection& other);

		const Jig::EdgeMesh::Vert* GetVert() const { return m_vert; }
		void SetVert(const Jig::EdgeMesh::Vert& vert) { m_vert = &vert; }

	private:
		const Jig::EdgeMesh::Vert* m_vert{};
	};
}
