#include "CellSoup.h"
#include "Field.h"
#include "math.h"
#include <windows.h>

#define EMPTY 0
#define CELL 1
#define WALL 2
#define BODY 3




void CellSoup::make_tiles()
{
	std::vector<tile> tmp;
	tile cur;
	cur.cell = NULL;
	cur.meat = 0;
	for (unsigned int row = 0; row < rows; row++) {
		cur.row = row;
		if (row == 0) cur.obj_type = WALL;
		else {
			if (row == rows - 1) cur.obj_type = WALL;
			else cur.obj_type = EMPTY;
		}
		double x = (double)row / (double)rows * 3.14;
		double sigm = -cos(x);
		cur.sigm = sigm;

		for (unsigned int col = 0; col < cols; col++) {
			cur.col = col;
			tmp.push_back(cur);
		}
	}

	for (unsigned int i = 0; i < rows*cols; i++) tiles[i] = tmp[i];

	change_season();
}

void CellSoup::find_neighbors()
{
	tile *cur;
	for (int i = 0; i < tiles.size(); i++) {
		cur = &tiles[i];
		bool down = (cur->row == 0), top = (cur->row == rows - 1), left = (cur->col%cols == 0), right = (cur->col%cols == cols - 1), offset = ((rows - cur->row - 1) % 2) == 1;

		int n_col, n_row;

		//Direction 'Right'
		n_row = cur->row;
		if (right) n_col = 0;
		else n_col = cur->col + 1;
		cur->neighbors[0] = &tiles[n_col + n_row*cols];

		//Direction 'Down-Right', 'Down-Left'
		if (down) n_row = rows - 1;
		else n_row = cur->row - 1;
		if (offset) {
			if (right) n_col = 0;
			else n_col = cur->col + 1;
			cur->neighbors[1] = &tiles[n_col + n_row*cols];
			n_col = cur->col;
			cur->neighbors[2] = &tiles[n_col + n_row*cols];
		}
		else {
			n_col = cur->col;
			cur->neighbors[1] = &tiles[n_col + n_row*cols];
			if (left) n_col = cols - 1;
			else n_col = cur->col - 1;
			cur->neighbors[2] = &tiles[n_col + n_row*cols];
		}

		//Direction 'Left'

		n_row = cur->row;
		if (left) n_col = cols - 1;
		else n_col = cur->col - 1;
		cur->neighbors[3] = &tiles[n_col + n_row*cols];

		//Directions 'Up-Left' and 'Up-Right'
		if (top) n_row = 0;
		else n_row = cur->row + 1;
		if (offset) {
			n_col = cur->col;
			cur->neighbors[4] = &tiles[n_col + n_row*cols];
			if (right) n_col = 0;
			else n_col = cur->col + 1;
			cur->neighbors[5] = &tiles[n_col + n_row*cols];
		}
		else {
			if (left) n_col = cols - 1;
			else n_col = cur->col - 1;
			cur->neighbors[4] = &tiles[n_col + n_row*cols];
			n_col = cur->col;
			cur->neighbors[5] = &tiles[n_col + n_row*cols];
		}
	}
}

CellSoup::CellSoup(unsigned int rows, unsigned int cols, Field *Graphics, int sun, int radiation, int minerals, int during)
{
	change_style = false;
	is_run = false;
	free_field = false;
	b_make_life = false;
	field_empty = true;

	this->Graphics = Graphics;
	this->rows = rows;
	this->cols = cols;
	this->sun = sun;
	this->radiation = radiation;
	this->minerals = minerals;
	this->season_during = during;
	this->graph_style = 0;
	this->step_ctr = 0;
	this->change_style = false;

	season_ctr = 3;
	seasons_differents[0] = 1.0;
	seasons_differents[1] = 0.8;
	seasons_differents[2] = 0.6;
	seasons_differents[3] = 0.9;

	cells.resize(rows*cols);
	tiles.resize(rows*cols);
	make_tiles();
	find_neighbors();
}

CellSoup::~CellSoup()
{
}

void CellSoup::obj_change(unsigned int row, unsigned int col, int new_type, cell *new_cell)
{
	tile *cur = &tiles[row*cols + col];
	cur->obj_type = new_type;
	cur->cell = new_cell->ctr;
	//tile_color_chаnge(&tiles[row * col + col]);
}

void CellSoup::obj_change(unsigned int ctr, int new_type, cell *new_cell)
{
	tiles[ctr].obj_type = new_type;
	tiles[ctr].cell = new_cell->ctr;
	tile_color_chаnge(&tiles[ctr]);
}

void CellSoup::obj_change(tile * cur, int new_type, cell *new_cell)
{
	cur->obj_type = new_type;
	if (new_cell == NULL) cur->cell = 0;
	else cur->cell = new_cell->ctr;
	//tile_color_chаnge(cur);

}

void CellSoup::obj_change(tile * cur, int new_type, int new_cell)
{
	cur->obj_type = new_type;
	if (new_cell == NULL) cur->cell = 0;
	else cur->cell = new_cell;

	//tile_color_chаnge(cur);
}

int CellSoup::get_sun(unsigned int row, unsigned int col)
{
	return tiles[row*cols + col].sun_lvl;
}

int CellSoup::get_sun(unsigned int ctr)
{
	return tiles[ctr].sun_lvl;
}

int CellSoup::change_minerals(int new_max)
{
	for (int i = 0; i < tiles.size(); i++) {
		if (tiles[i].sigm < 0) {
			tiles[i].mineral_lvl = (double)new_max * (-tiles[i].sigm);
			tiles[i].sun_lvl = 0;
		}
	}
	return 0;
}

int CellSoup::change_sun(int new_max)
{
	for (int i = 0; i < tiles.size(); i++) {
		if (tiles[i].sigm >= 0) {
			tiles[i].sun_lvl = (double)new_max * tiles[i].sigm;
			tiles[i].mineral_lvl = 0;
		}
	}
	return 0;
}

int CellSoup::change_rad(int new_max)
{
	for (int i = 0; i < tiles.size(); i++) tiles[i].radiation_lvl = (double)new_max * abs(tiles[i].sigm);
	return 0;
}

int CellSoup::change_season()
{
	season_ctr++;
	season_ctr %= 4;
	change_sun((double)sun * seasons_differents[season_ctr]);
	change_minerals((double)minerals * seasons_differents[season_ctr]);
	change_rad((double)radiation * seasons_differents[season_ctr]);
	return 0;
}

int CellSoup::get_minerals(unsigned int row, unsigned int col)
{
	return tiles[row*cols + col].mineral_lvl;
}

int CellSoup::get_minerals(unsigned int ctr)
{
	return tiles[ctr].mineral_lvl;
}

int CellSoup::get_radiation(unsigned int row, unsigned int col)
{
	return tiles[row*cols + col].radiation_lvl;
}

int CellSoup::get_radiation(unsigned int ctr)
{
	return tiles[ctr].radiation_lvl;
}

void CellSoup::make_cell(tile *position, int *DNA, int *feno, int energy) {
	cell *cur;
	for (int i = 0; i < cells.size(); i++) {
		if (cells[i].energy == 0) {		//Ищем пустышку
			cur = &cells[i];
			cur->ctr = i;

			cur->dest = 0;
			cur->energy = energy;
			cur->meat = 1;
			cur->minerals = 1;
			cur->sun = 2;
			cur->craw = 0;
			cur->err_ctr = 0;
			cur->position = position;
			cur->p_ctr = 0;

			if (DNA == NULL) {			//Сюда заходим ТОЛЬКО при изначальном создании клеток
				srand(clock()+rand());
				for (int j = 0; j < 10; j++) cur->fenotype[j] = rand() % 64;	//Генерируем случайный фенотип
				for (int j = 0; j < 64; j++) cur->DNA[j] = (rand() + clock()) % 64;		//Генерируем слуайный геном

				obj_change(position, CELL, cur);
			}
			else {			//Заходим сюда ТОЛЬКО при митозе
				srand(clock());
				for (int j = 0; j < 10; j++) cur->fenotype[j] = feno[j];
				for (int j = 0; j < 64; j++) cur->DNA[j] = DNA[j];

				if (rand() % 100 < position->radiation_lvl) {
					cur->DNA[rand() % 64] = rand() % 64;
					srand(clock());
					if (((rand() + clock()) % 100) < position->radiation_lvl) {
						cur->fenotype[(rand() + clock()) % 10] = (rand() + clock()) % 64;
					}
				}
				obj_change(position, CELL, cur);
			}

			return;
		}
		if (i == cells.size()) {
			printf("Owersize of cellsvector\n");
			return;
		}
	}
	
}

void CellSoup::cell_die(cell *daddy) {
	daddy->position->meat = daddy->energy / 2 + (daddy->meat + daddy->minerals + daddy->sun) * 12;
	daddy->energy = 0;
	obj_change(daddy->position, BODY, 0);
	//daddy->position = NULL;
	//Не стираю ДНК. Может, когда-нибудь запилю "воровство генов"
}

void CellSoup::tile_color_chаnge(tile* cur) {
	int R, G, B;
	switch (cur->obj_type)
	{
	case EMPTY:
		Graphics->change_tile_color(25, 25, 25, cur->row, cur->col);
		break;
	case CELL:
		if (graph_style == 0) {
			R = (double)cells[cur->cell].meat / (double)(cells[cur->cell].meat + cells[cur->cell].sun + cells[cur->cell].minerals) * 255;
			G = (double)cells[cur->cell].sun / (double)(cells[cur->cell].meat + cells[cur->cell].sun + cells[cur->cell].minerals) * 255;
			B = (double)cells[cur->cell].minerals / (double)(cells[cur->cell].meat + cells[cur->cell].sun + cells[cur->cell].minerals) * 255;
		}
		else {
			G = 0;
			R = (double)cells[cur->cell].energy / 500.0 * 255;
			B = 255.0 - ((double)cells[cur->cell].energy / 500.0 * 255);
		}

		Graphics->change_tile_color(R, G, B, cur->row, cur->col);
		break;
	case WALL:
		Graphics->change_tile_color(255, 255, 255, cur->row, cur->col);
		break;
	case BODY:
		Graphics->change_tile_color(150, 150, 150, cur->row, cur->col);
		break;
	default:
		break;
	}
}

void CellSoup::update_graphics()
{
	for (unsigned long long i = 0; i < tiles.size(); i++) {
		tile *cur = &tiles[i];
		
		tile_color_chаnge(cur);
	}
}

void CellSoup::make_life(int density)
{
	srand(clock());
	for (unsigned int i = cols; i < rows*cols - cols - 1; i++) {
		int r = (rand() + clock()/2) % 100;
		if (r <= density) make_cell(&tiles[i]);
	}
	update_graphics();
}

void CellSoup::updater() {
	while (true) {
		//Cловить мьютекс 
		update_graphics();
	}
}

void CellSoup::cell_step(cell* cur) {
	int AP = 1;
	while (AP > 0) {
		switch (cur->DNA[cur->p_ctr]) {
		case 1:
			turn(cur, 1);
			cur->p_ctr++;
			cur->p_ctr = cur->p_ctr % 64;
			cur->energy -= 5;
			AP--;
			break;

		case 2:
			turn(cur, 2);
			cur->p_ctr++;
			cur->p_ctr = cur->p_ctr % 64;
			cur->energy -= 5;
			AP--;
			break;

		case 3:
			turn(cur, 3);
			cur->p_ctr++;
			cur->p_ctr = cur->p_ctr % 64;
			cur->energy -= 5;
			AP--;
			break;

		case 4:
			turn(cur, 4);
			cur->p_ctr++;
			cur->p_ctr = cur->p_ctr % 64;
			cur->energy -= 5;
			AP--;
			break;

		case 5:
			turn(cur, 5);
			cur->p_ctr++;
			cur->p_ctr = cur->p_ctr % 64;
			cur->energy -= 5;
			AP--;
			break;

		case 10:	//Абс. условный переход "голод"
			AP--;
			cur->energy -= 2;
			is_hungry(cur, cur->DNA[cur->p_ctr + 1], cur->DNA[cur->p_ctr + 1], 0);

		case 12:	//абс. условный переход "пусто"
			AP--;
			look_empty(cur, cur->DNA[cur->p_ctr + 1], 0);
			cur->energy -= 2;
			break;

		case 13:	//отн. условный переход "пусто"
			AP--;
			look_empty(cur, cur->DNA[cur->p_ctr + 1], 1);
			cur->energy -= 2;
			break;

		case 15:	//отн. условный переход "голод"
			AP--;
			cur->energy -= 2;
			is_hungry(cur, cur->DNA[cur->p_ctr + 1], cur->DNA[cur->p_ctr + 1], 1);

		case 18:	//абс. условный переход "труп"
			AP--;
			look_body(cur, cur->DNA[cur->p_ctr + 1], 0);
			cur->energy -= 2;
			break;

		case 19:	//отн. условный переход "труп"
			AP--;
			look_body(cur, cur->DNA[cur->p_ctr + 1], 1);
			cur->energy -= 2;
			break;

		case 20: //Движение
			AP -= 5;
			if (move(cur, cur->DNA[cur->p_ctr + 1] % 6)) {
				cur->p_ctr += 2;
				cur->p_ctr = cur->p_ctr % 64;
				cur->energy -= 10;
				break;
			}
			else {
				AP++;
				cur->p_ctr += 2;
				cur->p_ctr = cur->p_ctr % 64;
				cur->energy -= 10;
				break;
			}

		case 25: //Атака в направлении
			AP -= 15;
			cur->energy -= 30;
			if (heat(cur, cur->DNA[cur->p_ctr + 1] % 6)) {
				cur->p_ctr += 2;
				cur->p_ctr = cur->p_ctr % 64;
				break;
			}
			else {
				AP += 5;
				cur->p_ctr += 2;
				cur->p_ctr = cur->p_ctr % 64;
				break;
			}

		case 30: //фотосинтез
			AP -= 15;
			cur->energy -= 20;
			if (photo(cur)) {
				cur->p_ctr++;
				cur->p_ctr = cur->p_ctr % 64;
				break;
			}
			else {
				cur->p_ctr++;
				cur->p_ctr = cur->p_ctr % 64;
				break;
			}

		case 33:	//условный переход по абс. адресу "свой - чужой"
			AP--;
			compare_feno(cur, cur->DNA[cur->p_ctr + 1], 0);
			cur->energy -= 2;
			break;

		case 34:	//условный переход по отн. адресу "свой - чужой"
			AP--;
			compare_feno(cur, cur->DNA[cur->p_ctr + 1], 1);
			cur->energy -= 2;
			break;

		case 35:	//абс. условный переход "Клетка - нет"
			AP--;
			look_cell(cur, cur->DNA[cur->p_ctr + 1], 0);
			cur->energy -= 2;
			break;

		case 36:	//отн. условный переход "Клетка - нет"
			AP--;
			look_cell(cur, cur->DNA[cur->p_ctr + 1], 1);
			cur->energy -= 2;
			break;

		case 40:	//Копать
			AP -= 15;
			dig(cur);
			cur->energy -= 15;
			cur->p_ctr++;
			break;

		case 45: //Усвоить
			AP -= 15;
			assim(cur);
			cur->p_ctr++;
			cur->p_ctr = cur->p_ctr % 64;
			cur->energy -= 20;
			break;

		case 50: //Митоз
			AP -= 15;
			cur->energy -= 15;
			if (mitose(cur, cur->DNA[cur->p_ctr + 1])) {
				cur->p_ctr += 2;
				cur->p_ctr = cur->p_ctr % 64;
				break;
			}
			else {
				cur->p_ctr += 2;
				cur->p_ctr = cur->p_ctr % 64;
				break;
			}

		default:
			AP--;
			cur->p_ctr++;
			cur->p_ctr = cur->p_ctr % 64;
			cur->energy -= 1;
			break;
		}
	}
	try_mutate(cur);
	dig(cur);
	if (cur->energy <= 0) cell_die(cur);
	cur->energy %= 250;
	//if (graph_style == 1) tile_color_chаnge(cur->position);
}
 
void CellSoup::step() {
	for (int i = 0; i < cells.size(); i++)
	{
		if (cells[i].energy == 0) continue;
		cell_step(&cells[i]);
		cells[i].err_ctr = 0;
	}
}

bool CellSoup::turn(cell * cur, int dir)
{
	cur->dest = (cur->dest + dir) % 6;
	return true;
}

bool CellSoup::move(cell *cur, int dir)
{
	turn(cur, dir);
	tile* new_tile = cur->position->neighbors[cur->dest];
	if (new_tile->obj_type == EMPTY) {
		obj_change(new_tile, CELL, cur);
		obj_change(cur->position, EMPTY, 0);
		cur->position = new_tile;
		return true;
	}
	else {
		cur->err_ctr++;
		return false;
	}
}

bool CellSoup::heat(cell * cur, int dir)
{
	turn(cur, dir);
	tile* attack_tile = cur->position->neighbors[cur->dest];
	switch (attack_tile->obj_type) {
	case CELL:
		cell_die(&cells[attack_tile->cell]);
		cur->energy += attack_tile->meat + 40;
		attack_tile->obj_type = EMPTY;
		//move(cur, 0);

		cur->meat++;													//!!!!!!!!!!!
		return true;
		break;

	case BODY:
		attack_tile->obj_type = EMPTY;											//!!!!!!!!!!!!!!!
		//move(cur, 0);
		cur->meat++;
		cur->energy += 60 + attack_tile->meat * 1.5;
		return true;
		break;

	default:
		cur->err_ctr++;
		return false;
	}
	//tile_color_chаnge(cur->position);
		
}

bool CellSoup::photo(cell * cur)
{
	cur->sun++;
	cur->energy += cur->position->sun_lvl * 12;

	//tile_color_chаnge(cur->position);
	return true;
}

bool CellSoup::look_cell(cell* cur, int offs, int type)
{
	if (cur->position->neighbors[cur->dest]->obj_type == CELL) {
		if (type == 0) cur->p_ctr = offs;
		else {
			cur->p_ctr += offs;
			cur->p_ctr %= 64;
		}
	}
	else {
		cur->p_ctr += 2;
		cur->p_ctr = cur->p_ctr % 64;
	}
	return true;
}

bool CellSoup::look_body(cell * cur, int offs, int type)
{
	if (cur->position->neighbors[cur->dest]->obj_type == BODY) {
		if (type == 0) cur->p_ctr = offs;
		else {
			cur->p_ctr += offs;
			cur->p_ctr %= 64;
		}
	}
	else {
		cur->p_ctr += 2;
		cur->p_ctr = cur->p_ctr % 64;
	}
	return true;
}

bool CellSoup::look_empty(cell * cur, int offs, int type)
{
	if (cur->position->neighbors[cur->dest]->obj_type == EMPTY) {
		if (type == 0) cur->p_ctr = offs;
		else {
			cur->p_ctr += offs;
			cur->p_ctr %= 64;
		}
	}
	else {
		cur->p_ctr += 2;
		cur->p_ctr = cur->p_ctr % 64;
	}
	return true;
}

bool CellSoup::compare_feno(cell *cur, int address, int type)
{
	if (cur->position->neighbors[cur->dest]->obj_type == CELL) {
		bool is_brother = true;
		for (int i = 0; i < 10; i++) {
			if (cur->DNA[cur->fenotype[i]] != cells[cur->position->neighbors[cur->dest]->cell].fenotype[i]) is_brother = false;
		}
		if (is_brother) {
			if (type) cur->p_ctr += type;
			else cur->p_ctr = type;
		}
		else {
			cur->p_ctr += 2;
		}
		cur->p_ctr %= 64;
		return true;
	}
	else {
		cur->p_ctr += 2;
		cur->p_ctr %= 64;
		return false;
	}
}

bool CellSoup::is_hungry(cell *cur, int operand, int address, int type)
{
	if (cur->energy < (float)operand / 63.0 * 250) {
		if (type == 0) cur->p_ctr = address;
		else {
			cur->p_ctr += address;
			cur->p_ctr %= 64;
		}
	}
	else {
		cur->p_ctr += 3;
		cur->p_ctr %= 64;
	}
	return false;
}

bool CellSoup::dig(cell* cur) {
	cur->craw += cur->position->mineral_lvl;
	cur->craw %=  200;

	//tile_color_chаnge(cur->position);
	return true;
}

bool CellSoup::assim(cell * cur)
{
	cur->minerals++;
	cur->energy += cur->craw * 15;										//!!!!!!!!!!!!!!!!!
	cur->craw = 0;

	//tile_color_chаnge(cur->position);
	return true;
}

bool CellSoup::mitose(cell * cur, int mothercare)
{
	int energy = (double)cur->energy * (double)(mothercare + 1.0) / 64.0;
	if (energy <= 60) return false;

	tile* baby_place = NULL;
	for each (tile* place in cur->position->neighbors) {
		if (place->obj_type == EMPTY) {
			baby_place = place;
			break;
		}
	}

	cur->energy -= energy;
	if (baby_place == NULL) return false;

	make_cell(baby_place, cur->DNA, cur->fenotype, energy);
	if (cur->energy == 0) cell_die(cur);
	return true;
}

bool CellSoup::try_mutate(cell * cur)
{
	srand(clock());
	if((rand() + clock()) % 100 < pow(cur->position->radiation_lvl/100, 5)){
		cur->DNA[(rand() + clock()) % 64] = (rand() + clock()) % 64;
	}
	return false;
}

void CellSoup::generator() {
	while (1) {
		if (change_style) {
			graph_style++;
			graph_style %= 2;
			change_style = false;
			Sleep(10);
			update_graphics();
		}

		if (is_run) {
			if (!field_empty) {
				step_ctr++;
				if (step_ctr % season_during == 0) change_season();
				step();
				update_graphics();
			}
			else is_run = false;
		}
		else {
			if (free_field) {
				Cl_17();
				free_field = false;
				field_empty = true;
			}
			if (b_make_life && field_empty) {
				make_life();
				b_make_life = false;
				field_empty = false;
			}
		}
	}
}

void CellSoup::Cl_17() {
	cells.clear();
	cells.resize(rows * cols);

	tiles.clear();
	tiles.resize(rows * cols);
	make_tiles();
	find_neighbors();
	update_graphics();
}

void CellSoup::ghosts() {
	for (int i = 0; i < tiles.size(); i++) {
		if (tiles[i].obj_type != CELL) continue;
		if (&tiles[i] != cells[tiles[i].cell].position) {
			int shit = 1;
		}
	}
}
