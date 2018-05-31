#include "MainView.h"
#include "PanelView.h"
#include "Window.h"

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include <gl/glu.h>

int main()
{
	Window window;
	window.setPosition({30, 50});
	window.setSize({ 1920 * 4 / 5 + 300, 1080 * 4 / 5 }); // TODO: Get from Window.
	window.setFramerateLimit(30);

	while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
			switch (event.type)
			{
			case sf::Event::Closed:
				window.close();
				break;

			case sf::Event::KeyPressed:
				if (event.key.code == sf::Keyboard::Escape)
					window.close();
				break;

			case sf::Event::MouseMoved:
				window.OnMouseMoved(event.mouseMove);
				break;
			}
		}

		window.Draw();

//		::Sleep(1);
    }

    return EXIT_SUCCESS;
}
