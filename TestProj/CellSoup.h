#pragma once
#include <SFML/Graphics.hpp>
#include "Field.h"
//#include "i_soup.h"

#define EMPTY 0
#define CELL 1
#define WALL 2
#define BODY 3

struct GL {
	int food_mode;				//����� ������� ������ �� ���. 0 - �� ������, 1 - ������
	int generate_mode;			//����� ������������� ���. 0 - ���������, 1 - ������������ �� ���� �������.
};

struct tile{									  //���������, ���������� �� ���� ����
	int obj_type;								  //����������� ���� �������, ������������ � �����.
												  
	float rad_sigm;								  //������������� ��������, ����������� ���-�� �������� � �����
	float sun_sigm;								  //������������� ��������, ����������� ������������ �����
	float miner_sigm;							  //������������� ��������, ����������� ���-�� ��������� � �����.
	int sun_lvl;								  //���������� �������� ����������� � �����.
	int mineral_lvl;							  //���������� ���-�� ��������� � �����
	int radiation_lvl;							  //��������� �������� �������� � �����
	unsigned int row;							  //������ �����
	unsigned int col;							  //������� �����
	unsigned int cell;							  //������ ������, ����������� � �����. (cells[tile::cell] - ������ �� �����)
	int meat;									  //���-�� ��������, ����������� � �����
	tile* neighbors[6];							  //������ ���������� �� ������-������� �����
};												  

class CellSoup
{
public:
	class Cell;

public:
	std::vector<tile> tiles;					  //������ ������ �� ����
private:
	Field *Graphics;							  //��������� �� ��������� ������, ����������� �� �������
	bool is_run 								  //���� ��������� ���������: ��������� ��������.
		,change_style 							  //���� ��������� ���������: ������ ��������� ���������� ����������
		,free_field								  //���� ��������� ���������: �������� ����
		,b_make_life 							  //���� ��������� ���������: ������������� ��������� ��������� ����
		,field_empty 							  //���� ��������� ���������: ���� ������
		,one_step;								  //���� ��������� ���������: ��������� ���� ��� ���������
	std::vector<Cell> cells;					  //������ ������ �� ����
	unsigned int rows 							  //������ ����
		,cols;									  //������ ����
	int season_during 							  //���������� ��� ���������� ����������: ������������ ������
		,season_ctr 							  //���������� ��� ���������� ����������: ���-�� �������
		,graph_style 							  //���������� ��� ���������� ����������: ������� ����������� �����
		,max_energy;								  //���������� ��� ���������� ����������: ������������ ���-�� ������� � ������
	static int start_energy 					  //���������� ��� ���������� ����������: ��������� ������� ������
		,olding_start 							  //���������� ��� ���������� ����������: ������� ������ �������� ������
		,olding_speed 							  //���������� ��� ���������� ����������: �������� �������� ������
		,died;									  //���������� ��� ���������� ����������: ���-�� ������� �� ������� ��� ������
	float sun 									  //������� ������� ������������.
		,radiation 								  //������� ������� ��������.
		,minerals;								  //������� ������� ���������.
	float seasons_differents[4];				  //������ ������������ �������� ���������� ����������� ����� � ��������.
	unsigned long long step_ctr;				  //������� �����.
	
private:
	void make_tiles();							  //������� ������ ������
	void find_neighbors();						  //��������� ������� �������
	void Cl_17();								  //�������� ���������� � ���������
	int get_sun(								  //�������� ���-�� ������ �� �����
		unsigned int row						  //������ �����
		,unsigned int col						  //������� �����
	);											  
	int get_sun(								  //�������� ���-�� ������ �� �����
		unsigned int ctr						  //������ �����
	);											  
	int change_minerals(						  //�������� minerals
		int new_max								  //����� ��������
	);											  
	int change_sun(								  //�������� sun
		int new_max								  //����� ��������
	);											  
	int change_rad(								  //�������� radiation
		int new_max								  //����� ��������
	);											  
	int change_season();						  //����� ������
	int get_minerals(							  //�������� ���������� ��������� � �����
		unsigned int row 						  //������ �����
		,unsigned int col						  //������� �����
	);											  
	int get_minerals(							  //�������� ���������� ��������� � �����
		unsigned int ctr						  //������ �����
	);											  
	int get_radiation(							  //�������� ���������� �������� � �����
		unsigned int row 						  //������ �����
		, unsigned int col						  //������� �����
	);											  
	int get_radiation(							  //�������� ���������� ��������� � �����
		unsigned int ctr						  //������ �����
	);											  

	void tile_color_ch�nge(tile * cur);			  //�������� ���� �����
	void update_graphics();						  //�������� ����� ���� ������


	void updater();								  //����������� ���� ���������� �������
	void step();								  //��������� ��� ���������


public:
	void generator();							  //�������� ������� ����������

	CellSoup(
		unsigned int rows						  //���-�� ����� � ����
		, unsigned int cols						  //���-�� �������� � ����
		, Field *Graphics						  //��������� �� ����������� �����
		, int sun = 20							  //������� �������� ������������
		, int radiation = 50					  //������� �������� ��������
		, int minerals = 23						  //������� �������� ���������
		, int during = 1000						  //������� �������� ������������ ������
	);
	CellSoup() {}
	~CellSoup();
												  
	void init_actions();						  //���������������� ������ ���������� �� ������ ������

	void make_life(								  //������������� ������� ������
		int density = 30						  //��������� ���������� ������, %
	);
	void obj_change(							  //��������� ������� � �����
		unsigned int row						  //������ �����
		, unsigned int col						  //������� �����
		, int new_type							  //��� ������ ������� � �����
		, Cell *new_cell						  //����� ������ �����
	);	
	void obj_change(							  //��������� ������� � �����
		unsigned int ctr						  //������ �����
		, int new_type							  //��� ������ ������� � �����
		, Cell *new_cell						  //����� ������ �����
	);											  
	void obj_change(							  //��������� ������� � �����
		tile *cur								  //��������� �� ����
		, int new_type							  //��� ������ ������� � �����
		, Cell *new_cell						  //����� ������ �����
	);			

	friend class i_soup;


	class Cell 									  //����� ������
	{
	public:
		typedef bool(CellSoup::Cell:: *cell_action)(int, int, int);		//���-��������� �� ������� �������� ������
		static std::vector<int> empty;									//���������� ������ �������� ��������� ������
		static std::vector<int> bizy;									//����������� ������ �������� ������� ������
		static cell_action *cell_actions;								//��������� �� ������ �������� ������

	private:		//�������� ������
		CellSoup* Core;													//��������� �� �������� ����� ����������
		int meat;														//������� (�� �����)
		int AP;															//���� �������� ������
		int sun;														//������� ����������� ������
		int minerals;													//������� �������� ��������� ������
		int energy;														//���-�� ������� ������
		int craw;														//����� ��������� ������
		int dest;														//����������� ������
		int DNA[64];													//��� ������
		int linker[64];													//��������� ������
		int p_ctr;														//������ ��������� ����
		unsigned int ctr;												//������ ������
		int feromones[3];												//������� ������
		tile *position;													//��������� �� ����, ��� ��������� ������

	public:			//������������ � �����������
		Cell() {};
		Cell(CellSoup* Core, tile *position, int dir, int *DNA = NULL, int *fero = NULL, int *linker = NULL, int energy = 300, int type = 0);
		Cell(const Cell &c1);
		~Cell() {};

	private:		//������� ������
		void cell_die(bool old_daddy = true);			//������ ������
		bool turn(int dir, int, int);					//�������
		bool move(int dir, int, int);					//�������� ����� �� 1 ������
		bool heat(int dir, int, int);					//���� + �������� �����
		bool photo(int, int, int);						//����������
		bool is_cell(int offs, int type, int);			//���. ������� "������"
		bool is_body(int offs, int type, int);			//���. ������� "����"
		bool is_empty(int offs, int type, int);			//���. ������� "�����"
		bool give_energy(int oper, int, int);			//�������� ������� �� �����������
		bool compare_fero(int address, int type, int);	//���. ������� "����-�����"
		bool is_hungry(int operand, int address, int type);	//���. ������� "�����"
		bool dig(int, int, int);						//������ ��������
		bool assim(int offs, int type, int);			//������� ��������
		bool mitose(int mothercare, int, int);			//���� ���������
		bool change_fero(int op1, int op2, int op3);	//�������� ������� (�������, ������ 0 -> �������� ��� ����)
		bool energy_paying(int pay, int, int);			//�����. �������� ������� �������
		bool cmp_DNA(int confidence, int address, int type);

	public:												
		void operator()();								//��������� �������� ��������� ����
		void set_ctr(int val);							//�������� �������� ���
		int get_ctr();									//�������� ������ ������
		int get_meat();									//�������� ������� �����������
		int get_sun();									//�������� ������������������
		int get_minerals();								//�������� ������� ���������
		int get_energy();								//�������� ���-�� ������� ������
		int get_dest();									//�������� ����������� ������
		int get_craw();									//�������� ���� ��������� ������
		int get_p_ctr();								//�������� ������ ��������� ����
		int* get_DNA();									//�������� ��������� �� ���
		int* get_linker();								//�������� ��������� �� �������

		friend void CellSoup::init_actions();			
	};

	class Perlen {
		int cols, rows;
	private:
		float **minerals;
		float **sun;
		float **radiation;

	public:
		Perlen(int rows, int cols, bool full = true, float intensive = 0.5f);
		~Perlen();

		Perlen& operator+(Perlen s1);
	};
};


