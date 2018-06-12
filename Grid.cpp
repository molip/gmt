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
	return GetPoint(sf::Vector2f(gridPoint));
}

sf::Vector2f Grid::GetPoint(sf::Vector2f gridPoint) const
{
	return { float(m_borderX + gridPoint.x * m_cellSize), float(m_borderY + gridPoint.y * m_cellSize) };
}

sf::Vector2f Grid::GetGridPoint(sf::Vector2f point) const
{
	return { float((point.x - m_borderX) / m_cellSize), float((point.y - m_borderY) / m_cellSize) };
}

sf::Vector2i Grid::GetNearestGridPoint(sf::Vector2f point, float* distanceSquared) const
{
	sf::Vector2f gridPoint = GetGridPoint(point);
	sf::Vector2i intGridPoint;
	intGridPoint.x = std::min(std::max(0, int(0.5 + gridPoint.x)), m_cellsX);
	intGridPoint.y = std::min(std::max(0, int(0.5 + gridPoint.y)), m_cellsY);

	if (distanceSquared)
	{
		sf::Vector2f delta = gridPoint - sf::Vector2f(intGridPoint);
		*distanceSquared = delta.x * delta.x + delta.y * delta.y;
	}

	return intGridPoint;
}

