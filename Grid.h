#pragma once

#include <SFML/System.hpp>

namespace GMT
{
	class Grid
	{
	public:
		Grid(int widthPixels, int heightPixels, int cellSize);

		int GetCellCountX() const { return m_cellsX; }
		int GetCellCountY() const { return m_cellsY; }

		// Logical points.
		sf::Vector2f GetPoint(sf::Vector2i gridPoint) const;
		sf::Vector2i GetNearestGridPoint(sf::Vector2f point, float* distanceSquared) const;

	private:
		const int m_widthPixels;
		const int m_heightPixels;
		const int m_cellSize;

		const int m_cellsX, m_cellsY;
		const int m_borderX, m_borderY;
	};
}
