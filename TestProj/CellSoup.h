#pragma once
#include <SFML/Graphics.hpp>
#include "Field.h"
//#include "i_soup.h"


struct tile{
	int obj_type;

	double sigm;
	int sun_lvl;
	int mineral_lvl;
	int radiation_lvl;
	unsigned int row;
	unsigned int col;
	unsigned int cell;
	int meat;
	tile* neighbors[6];
};

struct cell{
	int meat;
	int sun;
	int minerals;

	int energy;
	int craw;
	int dest;
	int DNA[64];
	int p_ctr;
	unsigned int ctr;
	int fenotype[10];

	int err_ctr;

	tile *position;
};


class CellSoup
{
	Field *Graphics;
public:
	std::vector<tile> tiles;
private:
	bool is_run, change_style, free_field, b_make_life, field_empty;
	std::vector<cell> cells;
	unsigned int rows, cols;
	int sun, radiation, minerals, season_during, season_ctr, graph_style;
	double seasons_differents[4];
	unsigned long long step_ctr;
	
private:
	void make_tiles();
	void find_neighbors();
	int get_sun(unsigned int row, unsigned int col);
	int get_sun(unsigned int ctr);
	int change_minerals(int new_max);
	int change_sun(int new_max);
	int change_rad(int new_max);
	int change_season();
	int get_minerals(unsigned int row, unsigned int col);
	int get_minerals(unsigned int ctr);
	int get_radiation(unsigned int row, unsigned int col);
	int get_radiation(unsigned int ctr);
	void obj_change(tile * cur, int new_type, int new_cell);
	void obj_change(unsigned int row, unsigned int col, int new_type, cell *new_cell);
	void obj_change(unsigned int ctr, int new_type, cell *new_cell);
	void obj_change(tile *cur, int new_type, cell *new_cell);
	void make_cell(tile *position, int *DNA = NULL, int *feno = NULL, int energy = 300);
	void cell_die(cell *daddy);
	void tile_color_chаnge(tile * cur);
	void update_graphics();

	void cell_step(cell* cur);
	void updater();
	void step();
	bool turn(cell* cur, int dir);	//Поворот
	bool move(cell* cur, int dir);	//Движение вперёд на 1 клетку
	bool heat(cell* cur, int dir);	//Удар + движение вперёд
	bool photo(cell* cur);			//Фотосинтез
	bool look_cell(cell* cur, int offs, int type);	//усл. переход "клетка"
	bool look_body(cell* cur, int offs, int type);	//усл. переход "тело"
	bool look_empty(cell* cur, int offs, int type);	//усл. переход "пусто"
	bool compare_feno(cell *cur, int address, int type);
	bool is_hungry(cell *cur, int operand, int address, int type);
	bool dig(cell* cur);			//Копать минералы
	bool assim(cell* cur);			//Усвоить минералы
	bool mitose(cell* cur, int mothercare);
	bool try_mutate(cell* cur);

	void Cl_17();

	void ghosts();
public:
	void generator();
	CellSoup(unsigned int rows, unsigned int cols, Field *Graphics, int sun = 12, int radiation = 50, int minerals = 15, int during = 300);
	CellSoup() {}
	~CellSoup();
	
	void make_life(int density = 30);

	friend class i_soup;
};

