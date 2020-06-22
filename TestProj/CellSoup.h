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
	bool is_run, change_style, free_field, b_make_life, field_empty, one_step;
	std::vector<cell> cells;
	std::vector<int> empty;
	std::vector<int> bizy;
	int died;
	unsigned int rows, cols;
	int season_during, season_ctr, graph_style, max_energy, start_energy, olding_start, olding_speed;
	float sun, radiation, minerals;
	float seasons_differents[4];
	unsigned long long step_ctr;
	
private:
	void make_tiles();
	void find_neighbors();
	void Cl_17();
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
	void tile_color_ch�nge(tile * cur);
	void update_graphics();

	void make_cell(tile *position, int dir, int *DNA = NULL, int *feno = NULL, int energy = 300, int type = 0);
	void cell_die(cell *daddy, bool old_daddy = true);

	void cell_step(cell* cur);
	void updater();
	void step();
	bool turn(cell* cur, int dir);	//�������
	bool move(cell* cur, int dir);	//�������� ����� �� 1 ������
	bool heat(cell* cur, int dir);	//���� + �������� �����
	bool photo(cell* cur);			//����������
	bool look_cell(cell* cur, int offs, int type);	//���. ������� "������"
	bool look_body(cell* cur, int offs, int type);	//���. ������� "����"
	bool look_empty(cell* cur, int offs, int type);	//���. ������� "�����"
	bool give_energy(cell* cur, int oper);
	bool compare_feno(cell *cur, int address, int type);
	bool is_hungry(cell *cur, int operand, int address, int type);
	bool dig(cell* cur, int passive);			//������ ��������
	bool assim(cell* cur);			//������� ��������
	bool mitose(cell* cur, int mothercare);
	bool try_mutate(cell* cur);
	void energy_paying(cell *cur, int pay);

	void ghosts();

public:
	void generator();

	CellSoup(unsigned int rows, unsigned int cols, Field *Graphics, int sun = 12, int radiation = 50, int minerals = 15, int during = 300);
	CellSoup() {}
	~CellSoup();
	
	void make_life(int density = 30);

	friend class i_soup;
};

