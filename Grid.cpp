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

