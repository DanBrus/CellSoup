#include "CellSoup.h"
#include "Field.h"
#include "math.h"
#include <windows.h>
#include <iostream>

int CellSoup::olding_start = 0;
int CellSoup::start_energy = 0;
int CellSoup::olding_speed = 0;
int CellSoup::died = 0;

std::vector<int> CellSoup::Cell::empty = { 0 };
std::vector<int> CellSoup::Cell::bizy = { 0 };
CellSoup::Cell::cell_action *CellSoup::Cell::cell_actions = NULL;


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
		double xsun = ((double)rows - (double)row) / (double)rows * 2.2;
		double xmin = (double)row / (double)rows * 2.2;
		double sigmsun = cos(xsun);
		double sigmmin = cos(xmin);
		
		if (sigmsun > 0)
			cur.sun_sigm = sigmsun;
		else
			cur.sun_sigm = 0;

		if (sigmmin > 0)
			cur.miner_sigm = sigmmin;
		else
			cur.miner_sigm = 0;

		cur.rad_sigm = sigmmin + sigmsun;

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
		bool down = (cur->row == 0), top = (cur->row == rows - 1), right = (cur->col%cols == 0), left = (cur->col%cols == cols - 1), offset = ((rows - cur->row) % 2) == 0;

		int n_col, n_row;

		//Direction 'Right'
		n_row = cur->row;
		if (right) n_col = cols - 1;
		else n_col = cur->col - 1;
		cur->neighbors[0] = &tiles[n_col + n_row*cols];

		//Direction 'Down-Right', 'Down-Left'
		if (down) n_row = rows - 1;
		else n_row = cur->row - 1;
		if (offset) {
			if (right) n_col = cols - 1;
			else n_col = cur->col - 1;
			cur->neighbors[1] = &tiles[n_col + n_row*cols];
			n_col = cur->col;
			cur->neighbors[2] = &tiles[n_col + n_row*cols];
		}
		else {
			n_col = cur->col;
			cur->neighbors[1] = &tiles[n_col + n_row*cols];
			if (left) n_col = 0;
			else n_col = cur->col + 1;
			cur->neighbors[2] = &tiles[n_col + n_row*cols];
		}

		//Direction 'Left'

		n_row = cur->row;
		if (left) n_col = 0;
		else n_col = cur->col + 1;
		cur->neighbors[3] = &tiles[n_col + n_row*cols];

		//Directions 'Up-Left' and 'Up-Right'
		if (top) n_row = 0;
		else n_row = cur->row + 1;
		if (offset) {
			n_col = cur->col;
			cur->neighbors[4] = &tiles[n_col + n_row*cols];
			if (right) n_col = cols - 1;
			else n_col = cur->col - 1;
			cur->neighbors[5] = &tiles[n_col + n_row*cols];
		}
		else {
			if (left) n_col = 0;
			else n_col = cur->col + 1;
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
	one_step = false;
	died = 0;

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
	seasons_differents[1] = 0.9;
	seasons_differents[2] = 0.8;
	seasons_differents[3] = 0.9;
	olding_start = 80;
	olding_speed = 1.2;

	for (int i = 0; i < rows*cols; i++) CellSoup::Cell::empty.push_back(i);
	cells.resize(rows*cols);
	tiles.resize(rows*cols);
	make_tiles();
	find_neighbors();
	init_actions();
}


void CellSoup::init_actions()
{
	CellSoup::Cell::cell_actions = new CellSoup::Cell::cell_action[20];
	for (int i = 0; i < 20; i++) CellSoup::Cell::cell_actions[i] = NULL;
	CellSoup::Cell::cell_actions[1] = &CellSoup::Cell::move;
	CellSoup::Cell::cell_actions[2] = &CellSoup::Cell::turn;
	CellSoup::Cell::cell_actions[4] = &CellSoup::Cell::is_hungry;
	CellSoup::Cell::cell_actions[5] = &CellSoup::Cell::is_empty;
	CellSoup::Cell::cell_actions[6] = &CellSoup::Cell::is_body;
	CellSoup::Cell::cell_actions[7] = &CellSoup::Cell::is_cell;
	CellSoup::Cell::cell_actions[8] = &CellSoup::Cell::change_fero;
	CellSoup::Cell::cell_actions[9] = &CellSoup::Cell::assim;
	CellSoup::Cell::cell_actions[10] = &CellSoup::Cell::heat;
	CellSoup::Cell::cell_actions[11] = &CellSoup::Cell::photo;
	CellSoup::Cell::cell_actions[12] = &CellSoup::Cell::dig;
	CellSoup::Cell::cell_actions[13] = &CellSoup::Cell::cmp_DNA;
	CellSoup::Cell::cell_actions[14] = &CellSoup::Cell::compare_fero;
	CellSoup::Cell::cell_actions[15] = &CellSoup::Cell::mitose;
	CellSoup::Cell::cell_actions[19] = &CellSoup::Cell::give_energy;
}

CellSoup::~CellSoup()
{
	delete CellSoup::Cell::cell_actions;
}

/*
void CellSoup::obj_change(tile * cur, int new_type, int new_cell)
{
	cur->obj_type = new_type;
	if (new_cell == NULL) cur->cell = 0;
	else cur->cell = new_cell;

	//tile_color_chаnge(cur);
}
*/
void CellSoup::obj_change(unsigned int row, unsigned int col, int new_type, Cell *new_cell)
{
	tile *cur = &tiles[row * cols + col];
	cur->obj_type = new_type;
	cur->cell = new_cell->get_ctr();
	//tile_color_chаnge(&tiles[row * col + col]);
}

void CellSoup::obj_change(unsigned int ctr, int new_type, Cell *new_cell)
{
	tiles[ctr].obj_type = new_type;
	tiles[ctr].cell = new_cell->get_ctr();
	tile_color_chаnge(&tiles[ctr]);
}

void CellSoup::obj_change(tile * cur, int new_type, Cell *new_cell)
{
	cur->obj_type = new_type;
	if (new_cell == NULL) cur->cell = 0;
	else cur->cell = new_cell->get_ctr();
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
			tiles[i].mineral_lvl = (double)new_max * (tiles[i].miner_sigm);
		}
	return 0;
}

int CellSoup::change_sun(int new_max)
{
	for (int i = 0; i < tiles.size(); i++) {
			tiles[i].sun_lvl = (double)new_max * tiles[i].sun_sigm;
		}

	return 0;
}

int CellSoup::change_rad(int new_max)
{
	for (int i = 0; i < tiles.size(); i++) tiles[i].radiation_lvl = (double)new_max * tiles[i].rad_sigm;
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

void CellSoup::tile_color_chаnge(tile* cur) {
	int R, G, B;
	if (graph_style == 2) {
		R = 50;
		G = cur->sun_lvl / sun * 255;
		B = cur->mineral_lvl / minerals * 255;
		Graphics->change_tile_color(R, G, B, cur->row, cur->col);

		return;
	}

	switch (cur->obj_type)
	{
	case EMPTY:
		Graphics->change_tile_color(25, 25, 25, cur->row, cur->col);
		break;
	case CELL:
		if (graph_style == 0) {
			double all_cell_resources = static_cast<double>(cells[cur->cell].get_meat() + cells[cur->cell].get_sun() + cells[cur->cell].get_minerals());

			R = static_cast<double>(cells[cur->cell].get_meat()) / all_cell_resources * 255;
			G = static_cast<double>(cells[cur->cell].get_sun()) / all_cell_resources * 255;
			B = static_cast<double>(cells[cur->cell].get_minerals()) / all_cell_resources * 255;
			/*
			if ((R > G) && (R > B)) {
				float k = 255 / R;
				R *= k;
				G *= k;
				B *= k;
			}
			else {
				if ((B > G) && (B > R)) {
					float k = 255 / B;
					R *= k;
					G *= k;
					B *= k;
				}
				else {
					float k = 255 / G;
					R *= k;
					G *= k;
					B *= k;
				}
			}
			R = (double)cells[cur->cell].meat / sqrt(pow(cells[cur->cell].meat, 2) + pow(cells[cur->cell].sun, 2) + pow(cells[cur->cell].minerals, 2)) * 255;
			G = (double)cells[cur->cell].sun / sqrt(pow(cells[cur->cell].meat, 2) + pow(cells[cur->cell].sun, 2) + pow(cells[cur->cell].minerals, 2)) * 255;
			B = (double)cells[cur->cell].minerals / sqrt(pow(cells[cur->cell].meat, 2) + pow(cells[cur->cell].sun, 2) + pow(cells[cur->cell].minerals, 2)) * 255;
			*/
			R %= 256;
			G %= 256;
			B %= 256;
		}
		if (graph_style == 1) {
			G = 0;
			R = (double)cells[cur->cell].get_energy() / max_energy * 255;
			B = 255.0 - ((double)cells[cur->cell].get_energy() / max_energy * 255);
		}
		if (graph_style == 3) {
			R = cells[cur->cell].get_fero()[0] * 8;
			G = cells[cur->cell].get_fero()[1] * 8;
			B = cells[cur->cell].get_fero()[2] * 8;
		}

		Graphics->change_tile_color(R, G, B, cur->row, cur->col);
		break;
	case WALL:
		Graphics->change_tile_color(255, 255, 255, 255, cur->row, cur->col);
		break;
	case BODY:
		Graphics->change_tile_color(150, 150, 150, 200, cur->row, cur->col);
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
		if (r <= density) Cell(this, &tiles[i], (rand() + clock())% 6, NULL, NULL, NULL, start_energy);
	}
	update_graphics();
}

void CellSoup::updater() {
	while (true) {
		//Cловить мьютекс 
		update_graphics();
	}
}

/*
void CellSoup::cell_step(cell* cur) {
	int AP = 15;
	while (AP > 0) {
		switch (cur->DNA[cur->p_ctr]) {
		case 1:
			cur->p_ctr++;
			cur->p_ctr = cur->p_ctr % 64;
			energy_paying(cur, 5);
			if (cur->energy <= 0)
				break;
			AP--;
			turn(cur, 1);
			break;

		case 2:
			cur->p_ctr++;
			cur->p_ctr = cur->p_ctr % 64;
			energy_paying(cur, 5);
			if (cur->energy <= 0)
				break;
			AP--;
			turn(cur, 2);
			break;

		case 3:
			cur->p_ctr++;
			cur->p_ctr = cur->p_ctr % 64;
			energy_paying(cur, 5);
			if (cur->energy <= 0)
				break;
			AP--;
			turn(cur, 3);
			break;

		case 4:
			cur->p_ctr++;
			cur->p_ctr = cur->p_ctr % 64;
			energy_paying(cur, 5);
			if (cur->energy <= 0)
				break;
			AP--;
			turn(cur, 4);
			break;

		case 5:
			cur->p_ctr++;
			cur->p_ctr = cur->p_ctr % 64;
			energy_paying(cur, 5);
			if (cur->energy <= 0)
				break;
			AP--;
			turn(cur, 5);
			break;

		case 10:	//Абс. условный переход "голод"
			AP--;
			energy_paying(cur, 2);
			if (cur->energy <= 0)
				break;
			is_hungry(cur, cur->DNA[(cur->p_ctr + 1) % 64], cur->DNA[(cur->p_ctr + 2) % 64], 0);

		case 12:	//абс. условный переход "пусто"
			AP--;
			energy_paying(cur, 2);
			if (cur->energy <= 0)
				break;
			is_empty(cur, cur->DNA[(cur->p_ctr + 1) % 64], 0);
			break;

		case 13:	//отн. условный переход "пусто"
			AP--;
			energy_paying(cur, 2);
			if (cur->energy <= 0)
				break;
			is_empty(cur, cur->DNA[(cur->p_ctr + 1) % 64], 1);
			break;

		case 15:	//отн. условный переход "голод"
			AP--;
			energy_paying(cur, 2);
			if (cur->energy <= 0)
				break;
			is_hungry(cur, cur->DNA[(cur->p_ctr + 1) % 64], cur->DNA[(cur->p_ctr + 2) % 64], 1);

		case 18:	//абс. условный переход "труп"
			AP--;
			energy_paying(cur, 2);
			if (cur->energy <= 0)
				break;
			is_body(cur, cur->DNA[(cur->p_ctr + 1) % 64], 0);
			break;

		case 19:	//отн. условный переход "труп"
			AP--;
			energy_paying(cur, 2);
			if (cur->energy <= 0)
				break;
			is_body(cur, cur->DNA[(cur->p_ctr + 1) % 64], 1);
			break;

		case 20: //Движение
			AP -= 5;
			energy_paying(cur, 10);
			if (cur->energy <= 0)
				break;
			if (move(cur, cur->DNA[(cur->p_ctr + 1) % 64] % 6)) {
				cur->p_ctr += 2;
				cur->p_ctr = cur->p_ctr % 64;
				break;
			}
			else {
				AP++;
				cur->p_ctr += 2;
				cur->p_ctr = cur->p_ctr % 64;
				break;
			}

		case 25: //Атака в направлении
			AP -= 15;
			energy_paying(cur, 30);
			if (cur->energy <= 0)
				break;
			if (heat(cur, cur->DNA[(cur->p_ctr + 1) % 64] % 6)) {
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
			energy_paying(cur, 20);
			if (cur->energy <= 0)
				break;
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
			energy_paying(cur, 2);
			if (cur->energy <= 0)
				break;
			compare_feno(cur, cur->DNA[(cur->p_ctr + 1) % 64], 0);
			break;

		case 34:	//условный переход по отн. адресу "свой - чужой"
			AP--;
			energy_paying(cur, 2);
			if (cur->energy <= 0)
				break;
			compare_feno(cur, cur->DNA[(cur->p_ctr + 1) % 64], 1);
			break;

		case 35:	//абс. условный переход "Клетка - нет"
			AP--;
			energy_paying(cur, 2);
			if (cur->energy <= 0)
				break;
			is_cell(cur, cur->DNA[(cur->p_ctr + 1) % 64], 0);
			break;

		case 36:	//отн. условный переход "Клетка - нет"
			AP--;
			energy_paying(cur, 2);
			if (cur->energy <= 0)
				break;
			is_cell(cur, cur->DNA[(cur->p_ctr + 1) % 64], 1);
			break;

		case 40:	//Копать
			AP -= 15;
			energy_paying(cur, 15);
			if (cur->energy <= 0)
				break;
			dig(cur, 0);
			cur->p_ctr++;
			break;

		case 45: //Усвоить
			AP -= 15;
			energy_paying(cur, 20);
			if (cur->energy <= 0)
				break;
			assim(cur);
			cur->p_ctr++;
			cur->p_ctr = cur->p_ctr % 64;
			break;

		case 47:	//Абс. усл. переход "сосед голоден"
			AP -= 1;
			energy_paying(cur, 2);
			if (cur->energy <= 0)
				break;
			if (cur->position->neighbors[cur->dest]->obj_type != CELL) break;
			is_hungry(&cells[cur->position->neighbors[cur->dest]->cell], cur->DNA[(cur->p_ctr + 1) % 64], cur->DNA[(cur->p_ctr + 2) % 64], 0);
			cur->p_ctr += 3;
			break;

		case 48:	//Отн. усл. переход "сосед голоден"
			AP -= 1;
			energy_paying(cur, 2);
			if (cur->energy <= 0)
				break;
			if (cur->position->neighbors[cur->dest]->obj_type != CELL) break;
			is_hungry(&cells[cur->position->neighbors[cur->dest]->cell], cur->DNA[(cur->p_ctr + 1) % 64], cur->DNA[(cur->p_ctr + 2) % 64], 1);
			cur->p_ctr += 3;
			break;

		case 50: //Митоз
			AP -= 15;
			energy_paying(cur, 15);
			if (cur->energy <= 0)
				break;
			if (mitose(cur, cur->DNA[(cur->p_ctr + 1) % 64])) {
				cur->p_ctr += 2;
				cur->p_ctr = cur->p_ctr % 64;
				break;
			}
			else {
				cur->p_ctr += 2;
				cur->p_ctr = cur->p_ctr % 64;
				break;
			}
		
		case 55:	//Обмен энергией
			AP -= 10;
			energy_paying(cur, 15);
			if (cur->energy <= 0)
				break;
			give_energy(cur, cur->DNA[(cur->p_ctr + 1) % 64]);
			cur->p_ctr += 2;
			cur->p_ctr = cur->p_ctr % 64;

		default:
			AP--;
			cur->p_ctr++;
			cur->p_ctr = cur->p_ctr % 64;
			energy_paying(cur, 1);
			break;
		}

		if (cur->energy <= 0)
			break;
	}
	try_mutate(cur);
	dig(cur, 0);
	if (cur->energy <= 0) cell_die(cur, false);
	if (cur->energy > max_energy)
		cur->energy = max_energy;
	//if (graph_style == 1) tile_color_chаnge(cur->position);
}
 */

void CellSoup::step() {
	for (int i = 0; i < Cell::bizy.size(); i++)
		cells[Cell::bizy[i]]();

	for (died; died > 0; died--) {
		Cell::bizy.pop_back();
	}
}

void CellSoup::generator() {
	while (1) {
		if (change_style) {
			graph_style++;
			graph_style %= 4;
			change_style = false;
			Sleep(10);
			update_graphics();
		}

		if (is_run) {
			if (!field_empty) {
				if (!one_step) {
					step_ctr++;
					if (step_ctr % season_during == 0) change_season();
					step();
					update_graphics();
				}
				else {
					step_ctr++;
					if (step_ctr % season_during == 0) change_season();
					step();
					update_graphics();
					one_step = false;
					is_run = false;
				}
			}
			else is_run = false;
		}
		else {
			if (!field_empty) {
				if (one_step) {
					step_ctr++;
					if (step_ctr % season_during == 0) change_season();
					step();
					update_graphics();
					one_step = false;
				}
			}
			else
				one_step = false;
			if (free_field) {
				Cl_17();
				free_field = false;
				field_empty = true;
				this->step_ctr = 0;
			}
			if (b_make_life && field_empty) {
				Cl_17();
				make_life();
				b_make_life = false;
				field_empty = false;
			}
			if (b_make_life && !field_empty)
				b_make_life = false;
		}
	}
}

void CellSoup::Cl_17() {
	cells.clear();
	cells.resize(rows * cols);

	tiles.clear();
	tiles.resize(rows * cols);

	Cell::bizy.clear();
	Cell::empty.clear();
	for (int i = 0; i < rows*cols; i++) Cell::empty.push_back(i);

	make_tiles();
	find_neighbors();
	update_graphics();
}























CellSoup::Cell::Cell(CellSoup * Core, tile * position, int dir, int * DNA, int * fero, int * linker, int energy, int type)
{
	this->Core = Core; this->dest = dir; this->energy = energy; this->meat = 1; this->minerals = 1; this->sun = 1; this->craw = 0; this->position = position; this->p_ctr = 0;
	Cell *cur;
	cur = &Core->cells[empty.back()];
	ctr = empty.back();
	empty.pop_back();

	if (died == 0)
		bizy.push_back(ctr);
	else {
		bizy.push_back(bizy.back());
		for (int i = 1; i < died; i++)
			bizy[bizy.size() - i - 1] = bizy[bizy.size() - i - 2];
		bizy[bizy.size() - died - 1] = ctr;
	}

	if (DNA == NULL) {			//Сюда заходим ТОЛЬКО при изначальном создании клеток
		srand(clock() + rand());
		for (int j = 0; j < 3; j++) this->feromones[j] = (rand() + clock()) % 31 + 1;				//Генерируем случайный набор феромонов
		for (int j = 0; j < 64; j++) this->linker[j] = (rand() + clock()) % 20;		//Генерируем случайный набор команд
		for (int j = 0; j < 64; j++) this->DNA[j] = (rand() + clock()) % 64;		//Генерируем слуайный геном
	}
	else {			//Заходим сюда ТОЛЬКО при митозе
		srand(clock());
		for (int j = 0; j < 3; j++) this->feromones[j] = fero[j];
		for (int j = 0; j < 64; j++) this->DNA[j] = DNA[j];
		for (int j = 0; j < 64; j++) this->linker[j] = linker[j];

		if ((rand() + clock()) % 100 < position->radiation_lvl) {
			DNA[rand() % 64] = rand() % 64;
			srand(clock());
			if (((rand() + clock()) % 100) < position->radiation_lvl / 3 * 2) {
				//fenotype[(rand() + clock()) % 10] = (rand() + clock()) % 64;
				this->linker[(rand() + clock()) % 64] = (rand() + clock()) % 20;
			}
		}
	}

	*cur = *this;
	Core->obj_change(position, CELL, cur);
}

CellSoup::Cell::Cell(const Cell &c1)
{
	Core = c1.Core; 
	dest = c1.dest; 
	energy = c1.energy;
	meat = c1.meat; 
	minerals = c1.minerals; 
	sun = c1.sun; 
	craw = c1.craw;
	position = c1.position; 
	p_ctr = c1.p_ctr;
	for (int j = 0; j < 3; j++) this->feromones[j] = c1.feromones[j];
	for (int j = 0; j < 64; j++) this->DNA[j] = c1.DNA[j];
	for (int j = 0; j < 64; j++) this->linker[j] = c1.linker[j];

	Core->obj_change(position, CELL, this);

	Core->cells[ctr] = *this;
}

const CellSoup::Cell* CellSoup::Cell::get_neightbor()
{
	return (position->neighbors[dest]->obj_type == CELL) ? &Core->cells[position->neighbors[dest]->cell] : nullptr;
}

void CellSoup::Cell::operator()()
{
	if (energy <= 0) {
		cell_die();
		return;
	}
	AP = 15;
	while (AP > 0) {
		if (cell_actions[linker[DNA[p_ctr]]] == NULL) {
			p_ctr++;
			p_ctr %= 64;
			AP -= 8;
			energy_paying(3, 0, 0);
		}
		else {
			if (!(this->*cell_actions[linker[DNA[p_ctr]]])(DNA[(p_ctr + 1) % 64], DNA[(p_ctr + 2) % 64], DNA[(p_ctr + 3) % 64]))
				break;
			p_ctr %= 64;
		}
	}
	if (energy > Core->max_energy) energy = Core->max_energy;
	if (energy <= 0) cell_die();
}

bool CellSoup::Cell::energy_paying(int pay, int, int)
{
	//int _pay = pay + pay * (pow(1.4, ((cur->meat + cur->sun + cur->minerals) / 5)) / 15);
	int _pay = pay;

	if (meat + sun + minerals > olding_start)
		_pay += (meat + sun + minerals - olding_start) * olding_speed;
	energy -= _pay;
	if (energy > 0)
		return true;
	else {
		//cell_die(true);
		return false;
	}
}

bool CellSoup::Cell::cmp_DNA(int confidence, int address, int type)
{
	if(!energy_paying(5, 0, 0))
		return false;
	AP--;

	int errors = 0;
	for (int i = 0; i < 64; i++) {
		if (DNA[i] != Core->cells[position->neighbors[dest]->cell].DNA[i])
			errors++;
		if (errors == confidence)
		{
			p_ctr = (p_ctr + 4) % 64;
			return true;
		}
	}

	if (type % 2 == 0) p_ctr = address;
	else {
		p_ctr += address;
		p_ctr %= 64;
	}
	return true;
}

bool CellSoup::Cell::turn(int dir, int, int)
{
	p_ctr += 2;
	AP--;
	if (!energy_paying(5, 0, 0))
		return false;

	dest = (dest + dir) % 6;
	return true;
}

bool CellSoup::Cell::move(int dir, int, int)
{
	p_ctr += 2;
	AP -= 5;
	if (!energy_paying(10, 0, 0))
		return false;

	turn(dir, 0, 0);
	tile* new_tile = position->neighbors[dest];
	if (new_tile->obj_type == EMPTY) {
		Core->obj_change(new_tile, CELL, this);
		Core->obj_change(position, EMPTY, 0);
		position = new_tile;
		return true;
	}
	else {
		return false;
	}
}

bool CellSoup::Cell::heat(int dir, int, int)
{
	p_ctr += 2;
	AP -= 15;
	if (!energy_paying(30, 0, 0))
		return false;

	turn(dir, 0, 0);
	tile* attack_tile = position->neighbors[dest];
	bool old_daddy;
	switch (attack_tile->obj_type) {
	case CELL:
		for (int i = 0; i < bizy.size(); i++) {
			if (bizy[i] == ctr) {
				old_daddy = false;
				break;
			}

			if (bizy[i] == Core->cells[attack_tile->cell].ctr) {
				old_daddy = true;
				break;
			}
		}

		Core->cells[attack_tile->cell].cell_die(old_daddy);

		energy += attack_tile->meat;
		attack_tile->obj_type = EMPTY;
		//move(cur, 0);

		meat++;													//!!!!!!!!!!!
		return true;
		break;

	case BODY:
		attack_tile->obj_type = EMPTY;											//!!!!!!!!!!!!!!!
																				//move(cur, 0);
		meat++;
		energy += attack_tile->meat * 1.5;
		return true;
		break;

	default:
		return true;
	}
	//tile_color_chаnge(cur->position);

}

bool CellSoup::Cell::photo(int, int, int)
{
	p_ctr += 2;
	AP -= 15;
	if (!energy_paying(20, 0, 0))
		return false;

	energy += position->sun_lvl * 7;
	sun++;
	//tile_color_chаnge(position);
	return true;
}

bool CellSoup::Cell::is_cell(int offs, int type, int)
{
	AP--;
	if (!energy_paying(2, 0, 0))
		return false;

	if (position->neighbors[dest]->obj_type == CELL) {
		if (type % 2 == 0) p_ctr = offs;
		else {
			p_ctr += offs;
			p_ctr %= 64;
		}
	}
	else {
		p_ctr +=3;
		p_ctr = p_ctr % 64;
	}
	return true;
}

bool CellSoup::Cell::is_body(int offs, int type, int)
{
	AP--;
	if (!energy_paying(2, 0, 0))
		return false;

	if (position->neighbors[dest]->obj_type == BODY) {
		if (type % 2 == 0) p_ctr = offs;
		else {
			p_ctr += offs;
			p_ctr %= 64;
		}
	}
	else {
		p_ctr += 3;
		p_ctr = p_ctr % 64;
	}
	return true;
}

bool CellSoup::Cell::is_empty(int offs, int type, int)
{
	AP--;
	if (!energy_paying(2, 0, 0))
		return false;

	if (position->neighbors[dest]->obj_type == EMPTY) {
		if (type % 2 == 0) p_ctr = offs;
		else {
			p_ctr += offs;
			p_ctr %= 64;
		}
	}
	else {
		p_ctr += 3;
		p_ctr = p_ctr % 64;
	}
	return true;
}

bool CellSoup::Cell::give_energy(int oper, int, int)
{
	p_ctr += 2;
	AP -= 10;
	if (!energy_paying(15, 0, 0))
		return false;

	tile* gift_addr = position->neighbors[dest];
	if (gift_addr->obj_type != CELL) return false;
	int gift = oper / 64 * 100 * (float)energy;
	Core->cells[gift_addr->cell].energy += gift;
	energy -= gift;

	return true;
}

bool CellSoup::Cell::compare_fero(int address, int type, int)
{
	AP--;
	if (!energy_paying(2, 0, 0))
		return false;
	//Если на тайле перед текущей клеткой клетка, то сравниваем феромоны
	if (position->neighbors[dest]->obj_type == CELL) {
		bool is_brother = true;
		for (int i = 0; i < 3; i++) {
			//if (DNA[this->feromones[i]] != Core->cells[position->neighbors[dest]->cell].feromones[i]) is_brother = false;
			if (this->feromones[i] != get_neightbor()->get_fero()[i]) is_brother = false;
		}
		if (is_brother) {																											//Если различий не обнаружено, то изменяем адрес след. гена:
			if (type % 2) p_ctr += address;																							//Случай относительной адресации
			else p_ctr = address;																									//Случай абсолютной адресации
		}
		else {																														//Если различия обнаружены, смещаем p_ctr на 3 пункта
			p_ctr += 3;
		}
		p_ctr %= 64;																												//Ограничение значения p_ctr
		return true;
	}
	else {
		p_ctr += 3;
		p_ctr %= 64;
		return true;
	}
}

bool CellSoup::Cell::is_hungry(int operand, int address, int type)
{
	p_ctr += 4;
	AP--;
	if (!energy_paying(2, 0, 0))
		return false;

	if (energy < (float)operand / 63.0 * 250) {
		if (type % 2 == 0) p_ctr = address;
		else {
			p_ctr += address;
			p_ctr %= 64;
		}
	}
	else {
		p_ctr += 3;
		p_ctr %= 64;
	}
	return true;
}

bool CellSoup::Cell::dig(int, int, int) 
{
	p_ctr++;
	AP -= 15;
	if (!energy_paying(15, 0, 0))
		return false;

	craw += (float)position->mineral_lvl;

	if (craw > 50)
		craw = 50;

	//tile_color_chаnge(position);
	return true;
}

bool CellSoup::Cell::assim(int, int, int)
{
	p_ctr++;
	AP -= 15;
	if (!energy_paying(10, 0, 0))
		return false;

	minerals++;
	energy += craw * 8;										//!!!!!!!!!!!!!!!!!
	craw = 0;

	//tile_color_chаnge(position);
	return true;
}

bool CellSoup::Cell::mitose(int mothercare, int, int)
{
	p_ctr += 2;
	AP -= 30;
	if (!energy_paying(15, 0, 0))
		return false;

	int energy = (double)this->energy * (double)(mothercare + 1.0) / 64.0;
	if (energy <= 30) return true;

	bool finding = true;
	tile* baby_place = NULL;

	for (int i = 0; i < 6; i++) {
		int j = (i + dest) % 6;
		if (position->neighbors[j]->obj_type == EMPTY) {
			baby_place = position->neighbors[j];
			finding = false;
			break;
		}
	}

	this->energy -= energy;
	if (baby_place == NULL) return false;
	int type;
	if (meat > sun) {
		if (meat > minerals) type = 1;
		else {
			if (sun > minerals) type = 0;
			else type = 2;
		}
	}
	else {
		if (sun > minerals) type = 0;
		else {
			if (meat > minerals) type = 1;
			else type = 2;
		}
	}
	Cell(Core, baby_place, dest, DNA, feromones, linker, energy, type);

	return true;
}

bool CellSoup::Cell::change_fero(int op1, int op2, int op3)
{
	AP--;
	if (!energy_paying(10, 0, 0))
		return false;
	p_ctr += 4;
	if (op1 != 0)
		feromones[0] = op1 % 31 + 1;
	if (op2 != 0)
		feromones[1] = op2 % 31 + 1;
	if (op3 != 0)
		feromones[2] = op3 % 31 + 1;



	return true;
}

void CellSoup::Cell::cell_die(bool old_daddy) {
	position->meat = energy / 2 + (meat + minerals + sun - 3) * 12;
	int i = 0;

	for (int i = 0; i < died; i++) {
		if (bizy[bizy.size() - 1 - i] == ctr) {
			for (int j = i; j > 0; j--)
				bizy[bizy.size() - 1 - j] = bizy[bizy.size() - j];
			bizy.pop_back();
			died--;
		}
	}

	for (int j = 0; j < bizy.size(); j++) {
		i = j;
		if (bizy[j] == ctr) break;
	}

	if (old_daddy) {
		empty.push_back(ctr);
		bizy[i] = bizy[bizy.size() - 1 - died];
		died++;
	}
	else {
		bizy[i] = bizy[bizy.size() - 1 - died];

		for (int j = died; j > 0; j--) {
			bizy[bizy.size() - j - 1] = bizy[bizy.size() - j];
		}
		bizy.pop_back();
		empty.push_back(ctr);
	}
	
	energy = 0;
	Core->obj_change(position, BODY, NULL);

}

void CellSoup::Cell::set_ctr(int val)
{
	ctr = val;
}

int CellSoup::Cell::get_ctr() const
{
	return ctr;
}

int  CellSoup::Cell::get_meat() const
{
	return meat;
}

int  CellSoup::Cell::get_sun() const
{
	return sun;
}

int  CellSoup::Cell::get_minerals() const
{
	return minerals;
}

int  CellSoup::Cell::get_energy() const
{
	return energy;
}

int CellSoup::Cell::get_dest() const
{
	return dest;
}

int CellSoup::Cell::get_craw() const
{
	return craw;
}

int CellSoup::Cell::get_p_ctr() const
{
	return p_ctr;
}

const int *CellSoup::Cell::get_DNA() const
{
	return DNA;
}

const int * CellSoup::Cell::get_linker() const
{
	return linker;
}

const int* CellSoup::Cell::get_fero() const
{
	return feromones;
}










CellSoup::Perlen::Perlen(int rows, int cols, bool full, float intensive) : cols(cols), rows(rows)
{
	minerals = new float*[rows];
	for (int i = 0; i < rows; i++) {
		minerals[i] = new float[cols];
		srand(clock());
		if (full == false)
			break;
		for (int j = 0; j < cols; i++) {
			if ((rand() + clock()) % 100 >= intensive * 100) {
				if ((rand() + clock()) % 2 == 0)
					minerals[i][j] = 0;
				else
					minerals[i][j] = 1;
			}
		}
	}

	sun = new float*[rows];
	for (int i = 0; i < rows; i++) {
		sun[i] = new float[cols];
		srand(clock());
		if (full == false)
			break;
		for (int j = 0; j < cols; i++) {
			if ((rand() + clock()) % 100 >= intensive * 100) {
				if ((rand() + clock()) % 2 == 0)
					sun[i][j] = 0;
				else
					sun[i][j] = 1;
			}
		}
	}

	radiation = new float*[rows];
	for (int i = 0; i < rows; i++) {
		minerals[i] = new float[cols];
		srand(clock());
		if (full == false)
			break;
		for (int j = 0; j < cols; i++) {
			if ((rand() + clock()) % 100 >= intensive * 100) {
				if ((rand() + clock()) % 2 == 0)
					minerals[i][j] = 1;
				else
					minerals[i][j] = 1;
			}
		}
	}
}

CellSoup::Perlen::~Perlen()
{
	for (int i = rows - 1; i >= 0; i--) {
		delete sun[i];
		delete minerals[i];
		delete radiation[i];
	}

	delete sun;
	delete minerals;
	delete radiation;
}

