#pragma once

#include "../Model/Element.h"
#include "../Model/VectorObject.h"

#include "Jig/EdgeMesh.h"

#include "libKernel/MinFinder.h"
#include "libKernel/EnumBitset.h"

namespace GMT
{
	class MainView;
}

namespace GMT::Tools
{
	class HitTester
	{
	public:
		enum class Option { Verts, EdgePoints, Edges, InternalEdges, Faces, Grid, WallThings, _Count };
		using Options = Kernel::EnumBitset<Option>;

		HitTester(const MainView& view, const Options& opts, float orthThreshold = 0);

		Model::ElementPtr HitTest(const sf::Vector2f& logPoint, const Model::VectorObject* special) const;

	protected:
		using MinFinder = Kernel::MinFinder<Model::ElementPtr, float>;
		struct Priority { enum Type { Grid, Face, Edge, Vert, WallThing }; };

		void HitTestObject(const Model::VectorObject& object, const sf::Vector2f& point, MinFinder& minFinder, float threshold) const;
		void HitTestEdge(const Model::VectorObject& object, const sf::Vector2f& point, const Jig::EdgeMesh::Edge& edge, const Jig::EdgeMesh::Edge& nextEdge, MinFinder& minFinder, float threshold) const;

		template <typename T>
		void HitTestEdgeLoop(const Model::VectorObject& object, const sf::Vector2f& point, const T& loop, MinFinder& minFinder, float threshold) const
		{
			for (const Jig::EdgeMesh::Edge& edge : loop)
				HitTestEdge(object, point, edge, T::GetNext(edge), minFinder, threshold);
		}

		const MainView& m_view;
		const Options m_opts;
		float m_orthThreshold;
	};
}
