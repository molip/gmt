#include "RenderContext.h"

using namespace GMT;

RenderContext::RenderContext(sf::RenderWindow& window, const Grid& grid) : m_window(window), m_grid(grid)
{
}

RenderContext::~RenderContext()
{
}
