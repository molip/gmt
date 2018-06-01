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

		sf::Vector2f GetPoint(sf::Vector2i gridPoint) const;

	private:
		const int m_widthPixels;
		const int m_heightPixels;
		const int m_cellSize;

		const int m_cellsX, m_cellsY;
		const int m_borderX, m_borderY;
	};
}
