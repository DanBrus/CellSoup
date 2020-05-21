#include "CellSimulator.h"
#include "Field.h"
#include "CellSoup.h"
#include "i_soup.h"
#include <thread>
#include <iostream>
#include <windows.h>


CellSimulator::CellSimulator()
{
	rows = 80;
	cols = 100;
	CellSoup Core(rows, cols, &Graphics);
	std::thread graphic(&CellSimulator::graphic_thread, this, rows, cols, &Core);
	//graphic.join();
	//Core.make_life();

	Core.generator();
}


CellSimulator::~CellSimulator()
{
}



void CellSimulator::graphic_thread(unsigned int rows, unsigned int cols, CellSoup *Core)
{
	sf::Vector2i window_size(1200, 600);
	sf::RenderWindow window(sf::VideoMode(window_size.x, window_size.y), "Cell Soup");

	Graphics = Field::Field(rows, cols, window.getSize());

	sf::Vector2f size = Graphics.get_size();

	i_soup Shell(Core, &Graphics, window_size, size, rows, cols);
	float zoom;
	while (window.isOpen())
	{
		// check all the window's events that were triggered since the last iteration of the loop

		sf::Vector2f offset;

		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type) {
			case sf::Event::Closed:
				window.close();
				break;

			case sf::Event::MouseMoved:
				Shell.mouse_move(event.mouseMove.x, event.mouseMove.y);
				break;

			case sf::Event::MouseWheelScrolled:
				zoom = pow(1.2, event.mouseWheelScroll.delta);
				Shell.zoom_field(zoom);
				break;

			case sf::Event::MouseButtonPressed:
				if (event.mouseButton.button == sf::Mouse::Left) {
					Shell.l_click();
				}
				break;

			case sf::Event::KeyPressed:
				Shell.key_pressed(event.key);

			case sf::Event::MouseButtonReleased:
				if (event.mouseButton.button == sf::Mouse::Left) {
					Shell.l_release();
				}
				break;

			default:
				break;
			}
		}
		window.clear(sf::Color::Black);
		//Define view
		// draw everything here...
		Shell.set_step_info();
		window.draw(Shell);
		window.draw(*Graphics.get_tiles());
		window.draw(*Graphics.get_borders());
		
		// end the current frame
		window.display();
	}
}
