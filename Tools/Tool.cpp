#include "Tool.h"

using namespace GMT;
using namespace GMT::Tools;


Tool::Tool(const MainView& view) : m_view(view)
{
}

Tool::~Tool() = default;

Model::ElementPtr Tool::HitTest(const sf::Vector2f& logPoint, const Model::VectorObject* special, const HitTester::Options& opts, bool threshold) const
{
	const float orthThreshold = threshold ? opts[HitTester::Option::InternalEdges] ? 0.25f : 0.5f : 100;
	return HitTester(m_view, opts, orthThreshold).HitTest(logPoint, special);
}
