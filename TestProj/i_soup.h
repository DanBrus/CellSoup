#pragma once
#include<SFML\Graphics.hpp>
#include "CellSoup.h"
#include "Field.h"



class i_soup : public sf::Drawable
{
	typedef void(i_soup:: *action)();
	class TextBox : public sf::Drawable {
	private:
		int type;
		sf::Text title;
		sf::Text content;
		std::wstring str;
		float value;
		sf::RectangleShape border;
		float *control_val;

	public:
		bool is_active;


		TextBox();
		TextBox(std::string title, float val, const sf::Font * font, sf::Vector2f global_size, sf::Vector2f pos, float offset);
		~TextBox() {};

		void input_char(char c);
		void set_control_val(float *val);
		bool is_hit(sf::Vector2f vect);
		void set_type(int type);
		void release();
		void set_title(std::string new_title);
		void set_val(float new_val);

	private:
		void click();
		void change_control_val();
		void stabilize_text();

	public:
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	};

	class Button : public sf::Drawable {
	private:
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
		~Button() {};

		bool is_hit(sf::Vector2f vect);
		void set_font(sf::Font *font);
		void release();
		void set_text(std::string str);
		void set_action(action act);
		void set_position(sf::Vector2f pos);
		void set_size(sf::Vector2f size);
		void stabilize_text();

	private:
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	};
	
	CellSoup *Core;

private:
	sf::View main_view, field_view, controls_view, info_view;
	sf::Vector2f mouse_pos, field_view_size;
	sf::Vector2i window_size;
	float cur_zoom, max_zoom;
	bool mouse_hold;
	std::vector<Button> buttons;
	Button *activeButton;
	std::vector<sf::Font> fonts;
	std::vector<TextBox> text_boxes;
	TextBox *activeTextBox;

public:
	i_soup(CellSoup *Core, sf::Vector2i window_size, sf::Vector2f size, int rows, int cols);
	~i_soup();

	bool is_field(sf::Vector2f pos);
	bool is_controls(sf::Vector2f pos);
	bool is_info(sf::Vector2f pos);
	void move_field(sf::Vector2f offset);
	void zoom_field(float zoom);
	void mouse_move(float x, float y);
	void l_click();
	void l_release();
	void key_pressed(sf::Event::KeyEvent key);
	sf::Vector2f global_to_local(sf::Vector2f vect, const sf::View local);

private:
	void release_text_box();
	void start_simulation();
	void stop_simulation();
	void make_life();
	void clean_field();
	void change_mode();

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};

