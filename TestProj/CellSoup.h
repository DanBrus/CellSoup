#pragma once
#include <SFML/Graphics.hpp>
#include "Field.h"
//#include "i_soup.h"

#define EMPTY 0
#define CELL 1
#define WALL 2
#define BODY 3

struct GL {
	int food_mode;				//Режим влияния клеток на еду. 0 - Не влияют, 1 - влияют
	int generate_mode;			//Режим распределения еды. 0 - статичный, 1 - динамический по шуму Перлина.
};

struct tile{									  //Структура, отвечающая за тайл поля
	int obj_type;								  //Обозначение типа объекта, находящегося в тайле.
												  
	float rad_sigm;								  //Относительная велисина, указывающая кол-во радиации в тайле
	float sun_sigm;								  //Относительная велисина, указывающая освещённость тайла
	float miner_sigm;							  //Относительная величина, указывающая кол-во минералов в тайле.
	int sun_lvl;								  //Абсолютная величина освещнности в тайле.
	int mineral_lvl;							  //Абсолютное кол-во минералов в тайле
	int radiation_lvl;							  //Абсолютна величина радиации в тайле
	unsigned int row;							  //Строка тайла
	unsigned int col;							  //Столбец тайла
	unsigned int cell;							  //Индекс клетки, находящейся в тайле. (cells[tile::cell] - клетка из тайла)
	int meat;									  //Кол-во органики, находящееся в тайле
	tile* neighbors[6];							  //Массив указателей на клеток-соседей тайла
};												  

class CellSoup
{
public:
	class Cell;

public:
	std::vector<tile> tiles;					  //Вектор тайлов на поле
private:
	Field *Graphics;							  //Указатель на экземпляр класса, отвечающего за графику
	bool is_run 								  //Флаг состояния симуляции: Симуляция запущена.
		,change_style 							  //Флаг состояния симуляции: Смнить выводимую графически информацию
		,free_field								  //Флаг состояния симуляции: Очистить поле
		,b_make_life 							  //Флаг состояния симуляции: Сгенерировать стартовые параметры поля
		,field_empty 							  //Флаг состояния симуляции: Поле пустое
		,one_step;								  //Флаг состояния симуляции: Совершить один шаг симуляции
	std::vector<Cell> cells;					  //Вектор клеток на поле
	unsigned int rows 							  //Высота поля
		,cols;									  //Ширина поля
	int season_during 							  //Информация для управления симуляцией: Длительность сезона
		,season_ctr 							  //Информация для управления симуляцией: Кол-во сезонов
		,graph_style 							  //Информация для управления симуляцией: Текущий графический стиль
		,max_energy;								  //Информация для управления симуляцией: Макситальное кол-во энергии у клетки
	static int start_energy 					  //Информация для управления симуляцией: Начальная энергия клетки
		,olding_start 							  //Информация для управления симуляцией: Возраст начала старения клетки
		,olding_speed 							  //Информация для управления симуляцией: Скорость старения клетки
		,died;									  //Информация для управления симуляцией: Кол-во умерших за текущий шаг клеток
	float sun 									  //Базовый уровень освещённости.
		,radiation 								  //Базовый уровень радиации.
		,minerals;								  //Базовый уровень минералов.
	float seasons_differents[4];				  //Массив отноительных сезонных отклонений питательной среды и радиации.
	unsigned long long step_ctr;				  //Счётчик шагов.
	
private:
	void make_tiles();							  //Создать массив тайлов
	void find_neighbors();						  //Заполнить массивы соседей
	void Cl_17();								  //Очистить информацию о симуляции
	int get_sun(								  //Получить кол-во солнца на тайле
		unsigned int row						  //Строка тайла
		,unsigned int col						  //Столбец тайла
	);											  
	int get_sun(								  //Получить кол-во солнца на тайле
		unsigned int ctr						  //Индекс тайла
	);											  
	int change_minerals(						  //Изменить minerals
		int new_max								  //Новое значение
	);											  
	int change_sun(								  //Изменить sun
		int new_max								  //Новое значение
	);											  
	int change_rad(								  //Изменить radiation
		int new_max								  //Новое значение
	);											  
	int change_season();						  //Смена сезона
	int get_minerals(							  //Получить ноличество минералов в тайле
		unsigned int row 						  //Строка тайла
		,unsigned int col						  //Столбец тайла
	);											  
	int get_minerals(							  //Получить ноличество минералов в тайле
		unsigned int ctr						  //Индекс тайла
	);											  
	int get_radiation(							  //Получить ноличество радиации в тайле
		unsigned int row 						  //Строка тайла
		, unsigned int col						  //Столбец тайла
	);											  
	int get_radiation(							  //Получить ноличество минералов в тайле
		unsigned int ctr						  //Индекс тайла
	);											  

	void tile_color_chаnge(tile * cur);			  //Обновить цвет тайла
	void update_graphics();						  //Обновить цвета всех тайлов


	void updater();								  //Бесконечный цикл обновления графики
	void step();								  //Выполнить шаг симуляции


public:
	void generator();							  //Основная функция генератора

	CellSoup(
		unsigned int rows						  //Кол-во строк в поле
		, unsigned int cols						  //Кол-во столбцов в поле
		, Field *Graphics						  //Указатель на графический класс
		, int sun = 20							  //Базовое значение освещённости
		, int radiation = 50					  //Базовое значение радиации
		, int minerals = 23						  //Базовое значение минералов
		, int during = 1000						  //Базовое значение длительности сезона
	);
	CellSoup() {}
	~CellSoup();
												  
	void init_actions();						  //Инициализировать массив указателей на дейсви клетки

	void make_life(								  //Инициализация массива клеток
		int density = 30						  //Плотность размещения клеток, %
	);
	void obj_change(							  //Изменение объекта в тайле
		unsigned int row						  //Строка тайла
		, unsigned int col						  //Столбец тайла
		, int new_type							  //Тип нового объекта в тайле
		, Cell *new_cell						  //Новая клетка тайла
	);	
	void obj_change(							  //Изменение объекта в тайле
		unsigned int ctr						  //Индекс тайла
		, int new_type							  //Тип нового объекта в тайле
		, Cell *new_cell						  //Новая клетка тайла
	);											  
	void obj_change(							  //Изменение объекта в тайле
		tile *cur								  //Угазатель на тайл
		, int new_type							  //Тип нового объекта в тайле
		, Cell *new_cell						  //Новая клетка тайла
	);			

	friend class i_soup;


	class Cell 									  //Класс клетки
	{
	public:
		typedef bool(CellSoup::Cell:: *cell_action)(int, int, int);		//Тип-указатель на функцию действия клетки
		static std::vector<int> empty;									//Сатический массив индексов свободных клеток
		static std::vector<int> bizy;									//Статический массив индексов занятых клеток
		static cell_action *cell_actions;								//Указатель на массив действий клеток

	private:		//Закрытые данные
		CellSoup* Core;													//Указатель на основной класс генератора
		int meat;														//Счётчик (не помню)
		int AP;															//Очки действия клетки
		int sun;														//Счётчик фотосинтеза клетки
		int minerals;													//Счётчик поедания минералов клетки
		int energy;														//Кол-во энергии клетки
		int craw;														//Запас минералов клетки
		int dest;														//Направление клетки
		int DNA[64];													//ДНК клетки
		int linker[64];													//Линковщик клетки
		int p_ctr;														//Индекс активного гена
		unsigned int ctr;												//Индекс клетки
		int feromones[3];												//Фенотип клетки
		tile *position;													//Указатель на тайл, где находится клетка

	public:			//Конструкторы и деструкторы
		Cell() {};
		Cell(CellSoup* Core, tile *position, int dir, int *DNA = NULL, int *fero = NULL, int *linker = NULL, int energy = 300, int type = 0);
		Cell(const Cell &c1);
		~Cell() {};

	private:		//Дейсвия клетки
		void cell_die(bool old_daddy = true);			//Смерть клетки
		bool turn(int dir, int, int);					//Поворот
		bool move(int dir, int, int);					//Движение вперёд на 1 клетку
		bool heat(int dir, int, int);					//Удар + движение вперёд
		bool photo(int, int, int);						//Фотосинтез
		bool is_cell(int offs, int type, int);			//усл. переход "клетка"
		bool is_body(int offs, int type, int);			//усл. переход "тело"
		bool is_empty(int offs, int type, int);			//усл. переход "пусто"
		bool give_energy(int oper, int, int);			//Передать энергию по направлению
		bool compare_fero(int address, int type, int);	//Усл. переход "свой-чужой"
		bool is_hungry(int operand, int address, int type);	//Усл. переход "голод"
		bool dig(int, int, int);						//Копать минералы
		bool assim(int offs, int type, int);			//Усвоить минералы
		bool mitose(int mothercare, int, int);			//Дать потомство
		bool change_fero(int op1, int op2, int op3);	//Изменить фромоны (операнд, равный 0 -> оставить как было)
		bool energy_paying(int pay, int, int);			//Вспом. действие затраты энергии
		bool cmp_DNA(int confidence, int address, int type);

	public:												
		void operator()();								//Выполнить действие активного гена
		void set_ctr(int val);							//Изменить активный ген
		int get_ctr();									//Полусить индекс клетки
		int get_meat();									//Полусить счётчик хищничества
		int get_sun();									//Полусить счётчикфотосинтеза
		int get_minerals();								//Получить счётчик минералов
		int get_energy();								//Получить кол-во энергии клетки
		int get_dest();									//Получить направление клетки
		int get_craw();									//Получить апас минералов клетки
		int get_p_ctr();								//Получить индекс автивного гена
		int* get_DNA();									//Получить указатель на ДНК
		int* get_linker();								//Получить указатель на фенотип

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


