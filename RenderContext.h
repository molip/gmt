#pragma once

namespace sf
{
	class RenderWindow;
}

namespace GMT
{
	class Grid;

	class RenderContext
	{
	public:
		RenderContext(sf::RenderWindow& window, const Grid& grid);
		~RenderContext();

		sf::RenderWindow& GetWindow() { return m_window; }
		const Grid& GetGrid() { return m_grid; }
	private:
		sf::RenderWindow& m_window;
		const Grid& m_grid;
	};
}
