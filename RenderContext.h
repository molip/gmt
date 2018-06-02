#pragma once

namespace sf
{
	class RenderWindow;
}

namespace GMT
{
	class RenderContext
	{
	public:
		RenderContext(sf::RenderWindow& window);
		~RenderContext();

		sf::RenderWindow& GetWindow() { return m_window; }

	private:
		sf::RenderWindow& m_window;
	};
}
