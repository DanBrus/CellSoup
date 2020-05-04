#pragma once
#include <SFML/Graphics.hpp>
#include "Field.h"
#include "CellSoup.h"
class CellSimulator
{
private:
	CellSoup Core;
	Field Graphics;
	void graphic_thread(unsigned int rows, unsigned int cols, CellSoup *Core);

	int sizeH, sizeW, rows, cols;
	
public:

	int i;
	CellSimulator();
	~CellSimulator();
};

