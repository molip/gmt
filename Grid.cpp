#include "Grid.h"

using namespace GMT;

Grid::Grid(int widthPixels, int heightPixels, int cellSize) : 
	m_widthPixels(widthPixels), 
	m_heightPixels(heightPixels), 
	m_cellSize(cellSize),
	m_cellsX(widthPixels / cellSize),
	m_cellsY(heightPixels / cellSize),
	m_borderX((widthPixels - m_cellsX * cellSize) / 2),
	m_borderY((heightPixels - m_cellsY * cellSize) / 2)
{
}

sf::Vector2f Grid::GetPoint(sf::Vector2i gridPoint) const
{
	return { float(m_borderX + gridPoint.x * m_cellSize), float(m_borderY + gridPoint.y * m_cellSize) };
}

sf::Vector2i Grid::GetNearestGridPoint(sf::Vector2f point, float* distanceSquared) const
{
	sf::Vector2f point2(point.x - (float)m_borderX, point.y - (float)m_borderY);
	sf::Vector2i gridPoint(int(0.5 + point2.x / m_cellSize), int(0.5 + point2.y / m_cellSize));
	gridPoint.x = std::min(std::max(0, gridPoint.x), m_cellsX);
	gridPoint.y = std::min(std::max(0, gridPoint.y), m_cellsY);

	if (distanceSquared)
	{
		sf::Vector2f delta = point - GetPoint(gridPoint);
		*distanceSquared = delta.x * delta.x + delta.y + delta.y;
	}

	return gridPoint;
}

