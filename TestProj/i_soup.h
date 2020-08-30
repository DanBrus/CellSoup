#pragma once
#include <SFML\Graphics.hpp>
#include "CellSoup.h"
#include "Field.h"



class i_soup : public sf::Drawable
{
	typedef void(i_soup:: *action)();
	CellSoup *Core;
	Field *Graphics;

public:
	class InterfaceObject : public sf::Drawable {
	protected:
		int type;
	public:
		int get_type() {
			return type;
		}
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
		virtual void set_position(sf::Vector2f pos) = 0;
		virtual void set_size(float size) = 0;
		virtual bool is_hit(sf::Vector2f vect) = 0;
		virtual void release() = 0;
		virtual void input_char(char c) = 0;
		virtual void update() = 0;
	};

private:

	struct tb_context {
		std::string title;					//Заголовок информационного поля
		float value;						//Начальное значение поля (влияет на значение в ядре при инициализации)
		const sf::Font *font;				//Указатель на шрифт
		sf::Vector2f global_size;			//Размер прямоугольника, в который будет вписан объект
		sf::Vector2f pos;					//Расположение поля (опорная точка - верхний левый угол прямоуг-ка)
		float *control_val;					//Указатель на контрольное значение
		int is_int;							//Является ли контрольное значение целочисленным? (0 если нет)
		float offset;						//Отступ начала текстового поля по Х от начала заголовка поля
	};
	class TextBox : public i_soup::InterfaceObject {
	private:
		//int type = 1;
		int is_int;
		sf::Text title;
		sf::Text content;
		std::wstring str;
		float value;
		sf::RectangleShape border;
		float *control_val;

	public:
		bool is_active;


		TextBox();
		TextBox(i_soup::tb_context context);
		TextBox(std::string title, float val, const sf::Font * font, sf::Vector2f global_size, sf::Vector2f pos, float offset);
		~TextBox() {};

		void set_position(sf::Vector2f pos) override;
		void set_size(float size) override;
		void input_char(char c) override;
		void update() override;
		bool is_hit(sf::Vector2f vect);
		void release();

	private:
		void set_title(std::string new_title);
		void set_val(float new_val);
		void set_is_int(int type);
		void set_control_val(float *val);
		void click();
		void change_control_val();
		void stabilize_text();
		
	public:
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	};

	struct b_context {
		sf::Vector2f pos;					//Расположение поля (опорная точка - верхний левый угол прямоуг-ка)
		sf::Vector2f size;					//Размер кнопки
		std::string title;					//Текст кнопки
		action But_act;						//Указатель на исполняемую кнопкой функцию/метод
		sf::Font *font;						//Указатель на шрифт для текста
		i_soup *obj;						//Указатель на объект класса, исполняющего метод
	};
	class Button : public i_soup::InterfaceObject {
	private:
		//int type = 0;
		bool hold;
		sf::RectangleShape border;
		sf::Text text;
		action But_act;
		TextBox test_box;
		sf::String string;
		i_soup *obj;

	private:
		void click();
		void change_state();

	public:
		Button();
		Button(sf::Vector2f pos, sf::Vector2f size, std::string str, action But_act, sf::Font *font, i_soup *obj);
		Button(i_soup::b_context context);
		~Button() {};

		void set_size(float size) override;
		void input_char(char c) override;
		void update() override;
		bool is_hit(sf::Vector2f vect);
		void release();
		void set_position(sf::Vector2f pos) override;
		void set_size(sf::Vector2f size);

	private:
		void stabilize_text();
		void set_text(std::string str);
		void set_action(action act);
		void set_font(sf::Font *font);
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	};

	struct il_context {
		std::string title;					//Заголовок информационного поля
		int *control_value;					//Указатель на контролируемое значение
		enum {INT, FLOAT, ULLINT} type;		//Тип контролируемого значения
		sf::Vector2f pos;					//Позиция поля заголовка
		sf::Vector2f offset;				//Смещение информационного поля относительно заголовка (0,0) для размещения информации сразу после заголовка
		sf::Font *font;						//Указаель на шрифт для текста
	};
	class InfoLabel: public i_soup::InterfaceObject {

	};

	sf::View main_view, field_view, controls_view, info_view;
	sf::Vector2f mouse_pos, field_view_size;
	sf::Vector2i window_size;
	float cur_zoom, max_zoom;
	bool mouse_hold,is_mouse_still;
	int active_tile;
	std::vector<InterfaceObject*> controls_clean, controls_run, controls_stop;
	std::vector<Button> buttons;
	Button *activeButton;
	std::vector<sf::Font> fonts;
	std::vector<TextBox> text_boxes;
	TextBox *activeTextBox;

	std::vector<sf::Text> cell_info_title;
	std::vector<sf::Text> cell_info;	//0 - ID; 1 - energy; 2 - direction; 3 - meat; 4 -sun; 5 - minerals; 6 - craw
	std::vector<sf::Text> dna_info;
	std::vector<sf::Text> global_info;

public:
	i_soup(CellSoup *Core, Field *Graphics, sf::Vector2i window_size, sf::Vector2f size, int rows, int cols);
	~i_soup();

	bool is_field(sf::Vector2f pos);		//Check for field click
	bool is_controls(sf::Vector2f pos);		//Check for controls click
	bool is_info(sf::Vector2f pos);			//Check for info click
	void move_field(sf::Vector2f offset);	//Moving of field
	void zoom_field(float zoom);			//Zooming of field
	void mouse_move(float x, float y);		//Change mouse position
	void l_click();							//Tap to left mouse button
	void l_release();						//Release left mouse button
	void key_pressed(sf::Event::KeyEvent key);	//Tap keyboard button
	void set_step_info();

	sf::Vector2f global_to_local(sf::Vector2f vect, const sf::View local);	//Transform global coordinates to view local coordinates

private:
	void make_buttons(std::vector<b_context> *b_ctxts, sf::Vector2f start_pos, std::vector<InterfaceObject*> *cur_panel);
	void make_text_boxes(std::vector<tb_context>* tb_ctxts, sf::Vector2f start_pos, std::vector<InterfaceObject*> *cur_panel);
	void info_init();
	void controls_init();
	void release_text_box();
	void start_simulation();
	void stop_simulation();
	void make_life();
	void clean_field();
	void change_mode();
	void one_step();
	void print_active_tile();
	void update_zoom();

	void update_controls();

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};

