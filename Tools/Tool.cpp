#include "Tool.h"

using namespace GMT;
using namespace GMT::Tools;


Tool::Tool(const MainView& view) : m_view(view)
{
}

Tool::~Tool() = default;

Model::ElementPtr Tool::HitTest(const sf::Vector2f& logPoint, const Model::VectorObject* special, const HitTester::Options& opts) const
{
	return HitTester(m_view, opts).HitTest(logPoint, special);
}
