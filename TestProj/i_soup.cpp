#include "i_soup.h"
#include "CellSoup.h"
#include <Windows.h>
#include <iostream>
#pragma warning(disable : 4996)


i_soup::i_soup(CellSoup *Core, Field *Graphics, sf::Vector2i window_size, sf::Vector2f size, int rows, int cols)
{
	//Filling variables
	this->Core = Core;
	this->Graphics = Graphics;

	mouse_hold = false;
	is_mouse_still = false;
	cur_zoom = 1;
	active_tile = -1;
	this->window_size = window_size;
	mouse_pos = sf::Vector2f(0, 0);
	activeButton = NULL;
	activeTextBox = NULL;

	//Calculation support variabeld
	float relation = window_size.x * 0.83f / window_size.y *0.85f;
	if (size.x / size.y > size.y * 0.83f * window_size.x / 0.85f * window_size.y) {
		field_view_size = sf::Vector2f(size.y * relation, size.y);
		max_zoom = 1 / (float)rows * 15;
		if (rows < 15) max_zoom = 1;
	}
	else {
		field_view_size = sf::Vector2f(size.x, size.x / relation);
		max_zoom = 1 / (float)cols * 15;
		if (cols < 15) max_zoom = 1;
	}

	//Making views
	main_view = sf::View(sf::Vector2f(window_size.x / 2, window_size.y / 2), sf::Vector2f(window_size.x, window_size.y));
	field_view = sf::View(sf::Vector2f(size.x / 2, size.y / 2), field_view_size);
	controls_view = sf::View(sf::Vector2f(window_size.x * 0.14f / 2, window_size.y * 0.85f / 2), sf::Vector2f(window_size.x * 0.14f, window_size.y * 0.85f));
	info_view = sf::View(sf::Vector2f(window_size.x * 0.98f / 2, window_size.y * 0.11f / 2), sf::Vector2f(window_size.x * 0.98f, window_size.y * 0.11f));

	//Setting views
	main_view.setViewport(sf::FloatRect(0.0f, 0.0f, 1.0f, 1.0f));
	field_view.setViewport(sf::FloatRect(0.01f, 0.02f, 0.83f, 0.85f));
	controls_view.setViewport(sf::FloatRect(0.85f, 0.02f, 0.14f, 0.85f));
	info_view.setViewport((sf::FloatRect(0.01f, 0.88f, 0.98f, 0.11f)));
	
	controls_init();

	info_init();
}

i_soup::~i_soup()
{
}

void i_soup::info_init()
{
	sf::Text cur = sf::Text(std::string("EMPTY"), fonts[0], 14);
	cur.setPosition(sf::Vector2f(3.0f, 3.0f));
	cur.setString(std::string("Cell ID:"));
	cell_info_title.push_back(cur);

	cur.setPosition(sf::Vector2f(110.0f, 3.0f));
	cur.setString(std::string("Energy:"));
	cell_info_title.push_back(cur);

	cur.setPosition(sf::Vector2f(200.0f, 3.0f));
	cur.setString(std::string("Direction:"));
	cell_info_title.push_back(cur);

	cur.setPosition(sf::Vector2f(320.0f, 3.0f));
	cur.setString(std::string("Meat:"));
	cell_info_title.push_back(cur);

	cur.setPosition(sf::Vector2f(390.0f, 3.0f));
	cur.setString(std::string("Sun:"));
	cell_info_title.push_back(cur);

	cur.setPosition(sf::Vector2f(450.0f, 3.0f));
	cur.setString(std::string("Mineral:"));
	cell_info_title.push_back(cur);

	cur.setPosition(sf::Vector2f(550.0f, 3.0f));
	cur.setString(std::string("Craw:"));
	cell_info_title.push_back(cur);

	cur.setCharacterSize(14);
	cur.setPosition(sf::Vector2f(3.0f, 22.0f));
	cur.setString(std::string("DNA:"));
	cell_info_title.push_back(cur);

	cur.setCharacterSize(14);
	cur.setPosition(sf::Vector2f(120.0f, 22.0f));
	cur.setString(std::string("Active gen:"));
	cell_info_title.push_back(cur);

	cur.setCharacterSize(10);
	cur.setColor(sf::Color::Magenta);
	cur.setPosition(sf::Vector2f(8.0f, 37.0f));
	cur.setString(std::string("0   1   2   3   4   5   6   7   8   9   10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63"));
	cell_info_title.push_back(cur);


	cur.setColor(sf::Color::White);
	cur.setCharacterSize(12);
	cur.setPosition(sf::Vector2f(60.0f, 5.0f));
	cur.setString(std::string("00000"));
	cell_info.push_back(cur);

	cur.setPosition(sf::Vector2f(160.0f, 5.0f));
	cur.setString(std::string("00"));
	cell_info.push_back(cur);

	cur.setPosition(sf::Vector2f(280.0f, 5.0f));
	cur.setString(std::string("--"));
	cell_info.push_back(cur);

	cur.setPosition(sf::Vector2f(360.0f, 5.0f));
	cur.setString(std::string("00"));
	cell_info.push_back(cur);

	cur.setPosition(sf::Vector2f(420.0f, 5.0f));
	cur.setString(std::string("00"));
	cell_info.push_back(cur);

	cur.setPosition(sf::Vector2f(505.0f, 5.0f));
	cur.setString(std::string("00"));
	cell_info.push_back(cur);

	cur.setPosition(sf::Vector2f(590.0f, 5.0f));
	cur.setString(std::string("00"));
	cell_info.push_back(cur);

	cur.setPosition(sf::Vector2f(195.0f, 25.0f));
	cur.setString(std::string("--"));
	cell_info.push_back(cur);

	cur.setCharacterSize(10);
	for (int i = 0; i < 10; i++) {
		cur.setString(std::string("00"));
		cur.setPosition(sf::Vector2f(4.0f + 16.5f * i, 48));
		dna_info.push_back(cur);
	}

	for (int i = 10; i < 64; i++) {
		cur.setString(std::string("00"));
		cur.setPosition(sf::Vector2f(168.0f + 17 * (i - 10), 48));
		dna_info.push_back(cur);
	}

	cur.setCharacterSize(14);
	cur.setPosition(sf::Vector2f(670.0f, 3.0f));
	cur.setString(std::string("Step No:"));
	global_info.push_back(cur);

	cur.setCharacterSize(12);
	cur.setPosition(sf::Vector2f(730.0f, 5.0f));
	cur.setString(std::string("00000000"));
	global_info.push_back(cur);

	cur.setCharacterSize(14);
	cur.setPosition(sf::Vector2f(820.0f, 3.0f));
	cur.setString(std::string("Cells:"));
	global_info.push_back(cur);

	cur.setCharacterSize(12);
	cur.setPosition(sf::Vector2f(860.0f, 5.0f));
	cur.setString(std::string("00000"));
	global_info.push_back(cur);

	cur.setCharacterSize(14);
	cur.setPosition(sf::Vector2f(925.0f, 3.0f));
	cur.setString(std::string("Season No:"));
	global_info.push_back(cur);

	cur.setCharacterSize(12);
	cur.setPosition(sf::Vector2f(1000.0f, 5.0f));
	cur.setString(std::string("0"));
	global_info.push_back(cur);
}

void i_soup::make_buttons(std::vector<b_context>* b_ctxts, sf::Vector2f start_pos, std::vector<InterfaceObject*> *cur_panel)
{
	size_t start = buttons.size();
	for (int i = 0; i < b_ctxts->size(); i++) {
		(*b_ctxts)[i].pos = sf::Vector2f(start_pos.x, start_pos.y + i * ((*b_ctxts)[0].size.y + 5));
		Button tmp((*b_ctxts)[i]);
		buttons.push_back(tmp);
	}
	for (int i = start; i < buttons.size(); i++)
		cur_panel->push_back(&buttons[i]);
}

void i_soup::make_text_boxes(std::vector<tb_context>* tb_ctxts, sf::Vector2f start_pos, std::vector<InterfaceObject*> *cur_panel)
{
	size_t start = text_boxes.size();
	for (int i = 0; i < tb_ctxts->size(); i++) {
		(*tb_ctxts)[i].pos = sf::Vector2f(start_pos.x, start_pos.y + i * ((*tb_ctxts)[0].global_size.y + 5));
		TextBox tmp((*tb_ctxts)[i]);
		text_boxes.push_back(tmp);
	}
	for (int i = start; i < text_boxes.size(); i++)
		cur_panel->push_back(&text_boxes[i]);
}

void i_soup::controls_init()
{
	buttons.reserve(30);
	text_boxes.reserve(50);
	fonts.push_back(sf::Font());
	fonts[0].loadFromFile("PetscopWide.ttf");

	//Making interface for clean field
	//Making buttons

	std::vector<b_context> b_ctxts;
	b_context b_cur;
	b_cur.font = &fonts[0];
	b_cur.obj = this;
	b_cur.size = sf::Vector2f(90.0f, 30.0f);

	b_cur.title = "Make life";
	b_cur.But_act = &i_soup::make_life;
	b_ctxts.push_back(b_cur);

	make_buttons(&b_ctxts, sf::Vector2f(5, 5), &controls_clean);

	//Making TextBoxes

	std::vector<tb_context> tb_contexts;
	tb_context cur;
	cur.font = &fonts[0];
	cur.global_size = sf::Vector2f(150, 15);
	cur.offset = 110.0f;
	cur.is_int = 0;

	cur.title = std::string("Sun level");
	cur.control_val = &Core->sun;
	cur.value = 12.0f;
	tb_contexts.push_back(cur);

	cur.title = std::string("Mineral level");
	cur.control_val = &Core->minerals;
	cur.value = 15.0f;
	tb_contexts.push_back(cur);

	cur.title = std::string("Radiation level");
	cur.control_val = &Core->radiation;
	cur.value = 50.0f;
	tb_contexts.push_back(cur);

	cur.title = std::string("Season duration");
	cur.control_val = (float*)&Core->season_during;
	cur.is_int = 1;
	cur.value = 300.0f;
	tb_contexts.push_back(cur);

	cur.title = std::string("Summer factor");
	cur.control_val = &Core->seasons_differents[0];
	cur.is_int = 0;
	cur.value = 1.0f;
	tb_contexts.push_back(cur);

	cur.title = std::string("Autumn factor");
	cur.control_val = &Core->seasons_differents[1];
	cur.value = 0.8f;
	tb_contexts.push_back(cur);

	cur.title = std::string("Winter factor");
	cur.control_val = &Core->seasons_differents[2];
	cur.value = 0.6f;
	tb_contexts.push_back(cur);

	cur.title = std::string("Spring factor");
	cur.control_val = &Core->seasons_differents[3];
	cur.value = 0.9f;
	tb_contexts.push_back(cur);

	cur.title = std::string("Max cell energy");
	cur.control_val = (float*)&Core->max_energy;
	cur.value = 250.0f;
	cur.is_int = 1;
	tb_contexts.push_back(cur);

	cur.title = std::string("Start cell energy");
	cur.control_val = (float*)&Core->start_energy;
	cur.value = 300.0f;
	tb_contexts.push_back(cur);

	cur.title = std::string("Rows");
	cur.control_val = (float*)&Core->rows;
	cur.value = 80.0f;
	tb_contexts.push_back(cur);

	cur.title = std::string("Cols");
	cur.control_val = (float*)&Core->cols;
	cur.value = 100.0f;
	tb_contexts.push_back(cur);

	make_text_boxes(&tb_contexts, sf::Vector2f(5, 150), &controls_clean);

	//Making stop-sim interface
	//Making buttons
	b_ctxts.clear();
	
	b_cur.title = "Start";
	b_cur.But_act = &i_soup::start_simulation;
	b_ctxts.push_back(b_cur);

	b_cur.title = "One step";
	b_cur.But_act = &i_soup::one_step;
	b_ctxts.push_back(b_cur);

	b_cur.title = "Change mode";
	b_cur.But_act = &i_soup::change_mode;
	b_ctxts.push_back(b_cur);

	b_cur.title = "Clean field";
	b_cur.But_act = &i_soup::clean_field;
	b_ctxts.push_back(b_cur);

	make_buttons(&b_ctxts, sf::Vector2f(5, 5), &controls_stop);
	
	//Making TextBoxes
	tb_contexts.pop_back();
	tb_contexts.pop_back();
	make_text_boxes(&tb_contexts, sf::Vector2f(5, 150), &controls_stop);

	//Making run-sim interface
	//Making buttons

	b_ctxts.clear();

	b_cur.title = "Stop";
	b_cur.But_act = &i_soup::stop_simulation;
	b_ctxts.push_back(b_cur);

	b_cur.title = "Change mode";
	b_cur.But_act = &i_soup::change_mode;
	b_ctxts.push_back(b_cur);

	b_cur.title = "Clean field";
	b_cur.But_act = &i_soup::clean_field;
	b_ctxts.push_back(b_cur);

	make_buttons(&b_ctxts, sf::Vector2f(5, 5), &controls_run);

	//Making TextBoxes
	make_text_boxes(&tb_contexts, sf::Vector2f(5, 150), &controls_run);
}

bool i_soup::is_field(sf::Vector2f pos)
{
	if ((pos.x < 0.84f * window_size.x) && (pos.x > 0.01f * window_size.x) && (pos.y < 0.87f * window_size.y) && (pos.y > 0.02f * window_size.y)) {
		return true;
	}
	return false;
}

bool i_soup::is_controls(sf::Vector2f pos)
{
	if ((mouse_pos.x < 0.99f * window_size.x) && (mouse_pos.x > 0.85f * window_size.x) && (mouse_pos.y < 0.87f * window_size.y) && (mouse_pos.y > 0.02f * window_size.y)) {
		return true;
	}
	return false;
}

bool i_soup::is_info(sf::Vector2f pos)
{
	if ((mouse_pos.x < 0.99f * window_size.x) && (mouse_pos.x > 0.01f * window_size.x) && (mouse_pos.y < 0.99f * window_size.y) && (mouse_pos.y > 0.88f * window_size.y)) {
		return true;
	}
	return false;
}

void i_soup::move_field(sf::Vector2f offset)
{
	field_view.setCenter(field_view.getCenter().x + offset.x, field_view.getCenter().y + offset.y);
	sf::Vector2f offset_(0, 0);
	if (field_view.getCenter().x + field_view.getSize().x / 2 > field_view_size.x) offset_.x = field_view_size.x - (field_view.getCenter().x + field_view.getSize().x / 2);
	if (field_view.getCenter().x - field_view.getSize().x / 2 < 0) offset_.x =  -(field_view.getCenter().x - field_view.getSize().x / 2);

	if (field_view.getCenter().y + field_view.getSize().y / 2 > field_view_size.y) offset_.y = field_view_size.y - (field_view.getCenter().y + field_view.getSize().y / 2);
	if (field_view.getCenter().y - field_view.getSize().y / 2 < 0) offset_.y = -(field_view.getCenter().y - field_view.getSize().y / 2);

	field_view.setCenter(field_view.getCenter().x + offset_.x, field_view.getCenter().y + offset_.y);
}

void i_soup::zoom_field(float zoom)
{
	if (!is_field(mouse_pos)) return;

	float tmp = cur_zoom * zoom;

	if (tmp < max_zoom) zoom = max_zoom / cur_zoom;
	if (tmp > 1) zoom = 1 / cur_zoom;
	cur_zoom *= zoom;
	float offsetX = (1 - zoom) * (mouse_pos.x - (float)window_size.x * 0.01f - (float)window_size.x * 0.83 / 2) * field_view.getSize().x / ((float)window_size.x * 0.83f);
	float offsetY = (1 - zoom) * (mouse_pos.y - (float)window_size.y * 0.02f - (float)window_size.y * 0.85 / 2) * field_view.getSize().y / ((float)window_size.y * 0.85f);
	field_view.zoom(zoom);

	move_field(sf::Vector2f(offsetX, offsetY));
}

void i_soup::mouse_move(float x, float y)
{
	is_mouse_still = false;
	if (mouse_hold) {
		sf::Vector2f offset((mouse_pos.x - x) * field_view.getSize().x / ((float)window_size.x * 0.83f), (mouse_pos.y - y) * field_view.getSize().y / ((float)window_size.y * 0.85f));
		move_field(offset);
	}

	mouse_pos.x = x;
	mouse_pos.y = y;
}

void i_soup::l_click()
{
	bool hited = false;
	is_controls(mouse_pos);
	if (is_field(mouse_pos)) {
		mouse_hold = true;
		is_mouse_still = true;
	}
	if (is_controls(mouse_pos)) {
		/*
		for (int i = 0; i < buttons.size(); i++) {
			if (buttons[i].is_hit(global_to_local(mouse_pos, controls_view))) activeButton = &buttons[i];
		}
		for (int i = 0; i < text_boxes.size(); i++) {
			if (text_boxes[i].is_hit(global_to_local(mouse_pos, controls_view))) {
				if (activeTextBox != NULL) release_text_box();
				activeTextBox = &text_boxes[i];
				hited = true;
			}
		}
		*/
		std::vector<InterfaceObject*> *controls = &controls_stop;
		if (Core->field_empty)
			controls = &controls_clean;
		else
			if (Core->is_run)
				controls = &controls_run;
		for (int i = 0; i < (*controls).size(); i++) {
			if ((*controls)[i]->is_hit(global_to_local(mouse_pos, controls_view))) {
				switch ((*controls)[i]->get_type()) {
				case 0:
					activeButton = (Button*)(*controls)[i];
					break;

				case 1:
					if (activeTextBox != NULL) release_text_box();
					activeTextBox = (TextBox*)(*controls)[i];
					hited = true;
					break;

				default:
					break;
				}
			}
		}
	}
	if (!hited) release_text_box();
}

void i_soup::release_text_box()
{
	if (activeTextBox == NULL) return;
	activeTextBox->release();
	activeTextBox = NULL;
}

void i_soup::l_release()
{
	mouse_hold = false;

	update_controls();
	if (is_mouse_still) {
		is_mouse_still = false;
		if (is_field(mouse_pos) && !Core->is_run) {
			active_tile = Graphics->is_hit(global_to_local(mouse_pos, field_view));
			//printf("%d\n", &active_cell);
			//std::cout << active_tile << std::endl;
			print_active_tile();
		}
	}

	if (activeButton != NULL) activeButton->release();
	activeButton = NULL;
}

void i_soup::key_pressed(sf::Event::KeyEvent key)
{
	if (activeTextBox != 0) activeTextBox->input_char(key.code);
}

void i_soup::set_step_info()
{
	global_info[1].setString(std::to_string(Core->step_ctr));
	global_info[3].setString(std::to_string(CellSoup::Cell::bizy.size()));
	global_info[5].setString(std::to_string(Core->season_ctr));
}

sf::Vector2f i_soup::global_to_local(sf::Vector2f vect, const sf::View local)
{
	sf::Vector2f result(0, 0);
	result.x = (vect.x - (local.getViewport().width / 2 + local.getViewport().left) * (float)window_size.x) * local.getSize().x / (local.getViewport().width * (float)window_size.x) + local.getCenter().x;
	result.y = (vect.y - (local.getViewport().height / 2 + local.getViewport().top) * (float)window_size.y) * local.getSize().y / (local.getViewport().height *  (float)window_size.y) + local.getCenter().y;
	return result;
}

void i_soup::start_simulation()
{
	update_controls();
	active_tile = -1;
	print_active_tile();
	Core->is_run = true;
}

void i_soup::stop_simulation()
{
	update_controls();
	Core->is_run = false;
}

void i_soup::make_life()
{
	Graphics->change_size(Core->rows, Core->cols);
	update_controls();
	Core->is_run = false;
	Sleep(50);
	Core->free_field = false;
	Sleep(50);
	Core->b_make_life = true;
	Sleep(50);
	update_zoom();
}

void i_soup::clean_field()
{
	Core->is_run = false;
	Sleep(50);
	Core->free_field = true;
}

void i_soup::one_step()
{
	Core->one_step = true;
}

void i_soup::print_active_tile()
{
	CellSoup::Cell *cur;

	for (int i = 0; i < 8; i++)
		cell_info[i].setString(std::string("-"));
	for (int i = 0; i < 64; i++)
		dna_info[i].setString(std::string("00"));

	if (active_tile == -1)
		return;

	cur = &Core->cells[Core->tiles[active_tile].cell];
	const int *DNA = cur->get_DNA();
	const int *linker = cur->get_linker();
	switch (Core->tiles[active_tile].obj_type) {
	case 0:		//EMPTY
		break;

	case 1:		//CELL
		cell_info[0].setString(std::to_string(cur->get_ctr()));
		cell_info[1].setString(std::to_string(cur->get_energy()));
		cell_info[2].setString(std::to_string(cur->get_dest()));
		cell_info[3].setString(std::to_string(cur->get_meat()));
		cell_info[4].setString(std::to_string(cur->get_sun()));
		cell_info[5].setString(std::to_string(cur->get_minerals()));
		cell_info[6].setString(std::to_string(cur->get_craw()));
		cell_info[7].setString(std::to_string(cur->get_p_ctr()));

		for (int i = 0; i < 64; i++)
			dna_info[i].setString(std::to_string(linker[DNA[i]]));
		break;

	case 2:		//BODY
		cell_info[3].setString(std::to_string(Core->tiles[active_tile].meat));
		break;

	case 3:		//WALL

		break;
	}

	return;
}

void i_soup::update_zoom()
{
	sf::Vector2f size = Graphics->get_size();
	float relation = window_size.x * 0.83f / window_size.y *0.85f;
	if (size.x / size.y > size.y * 0.83f * window_size.x / 0.85f * window_size.y) {
		field_view_size = sf::Vector2f(size.y * relation, size.y);
		max_zoom = 1 / (float)Core->rows * 15;
		if (Core->rows < 15) max_zoom = 1;
	}
	else {
		field_view_size = sf::Vector2f(size.x, size.x / relation);
		max_zoom = 1 / (float)Core->cols * 15;
		if (Core->cols < 15) max_zoom = 1;
	}

	field_view.setSize(field_view_size);
}

void i_soup::change_mode()
{
	Core->change_style = true;
}

void i_soup::draw(sf::RenderTarget & target, sf::RenderStates states) const
{

	sf::RectangleShape info_rect(sf::Vector2f(window_size.x * 0.98f, window_size.y * 0.11f));
	info_rect.setFillColor(sf::Color(120, 120, 255, 90));
	info_rect.setOutlineThickness(-5.f);
	info_rect.setOutlineColor(sf::Color(30, 90, 255, 255));

	sf::RectangleShape field_border(sf::Vector2f(window_size.x * 0.83f, window_size.y * 0.85f));
	field_border.setFillColor(sf::Color(0, 0, 0, 0));
	field_border.setPosition(window_size.x * 0.01f, window_size.y * 0.02f);
	field_border.setOutlineColor(sf::Color(255, 255, 255, 255));
	field_border.setOutlineThickness(5.f);

	target.setView(main_view);
	target.draw(field_border);

	target.setView(info_view);
	//target.draw(info_rect);
	for (int i = 0; i < cell_info_title.size(); i++)
		target.draw(cell_info_title[i]);
	for (int i = 0; i < cell_info.size(); i++)
		target.draw(cell_info[i]);
	for (int i = 0; i < 64; i++)
		target.draw(dna_info[i]);
	for (int i = 0; i < global_info.size(); i++)
		target.draw(global_info[i]);

	target.setView(controls_view);
	
	/*
	for (int i = 0; i < text_boxes.size(); i++)
		target.draw(text_boxes[i]);
	for(int i = 0; i < buttons.size(); i++)
		target.draw(buttons[i]);
	*/
	if (Core->field_empty)
		for (int i = 0; i < controls_clean.size(); i++)
			target.draw(*controls_clean[i]);
	else
	{
		if (Core->is_run)
			for (int i = 0; i < controls_run.size(); i++)
				target.draw(*controls_run[i]);
		else
			for (int i = 0; i < controls_stop.size(); i++)
				target.draw(*controls_stop[i]);
	}


	target.setView(field_view);
}

void i_soup::update_controls()
{
	for (int i = 0; i < controls_clean.size(); i++) {
		controls_clean[i]->update();
	}
	for (int i = 0; i < controls_run.size(); i++) {
		controls_run[i]->update();
	}
	for (int i = 0; i < controls_stop.size(); i++) {
		controls_stop[i]->update();
	}
}








i_soup::Button::Button() 
{
	type = 0;
	border.setFillColor(sf::Color(150, 150, 150, 100));
	border.setOutlineColor(sf::Color(180, 180, 180, 210));
	border.setOutlineThickness(3.0f);
	hold = false;
}

i_soup::Button::Button(sf::Vector2f pos, sf::Vector2f size, std::string str, action But_act, sf::Font *font, i_soup *obj)
{
	type = 0;
	this->obj = obj;
	border = sf::RectangleShape(size);
	border.setPosition(pos);
	border.setFillColor(sf::Color(150, 150, 150, 100));
	border.setOutlineColor(sf::Color(180, 180, 180, 210));
	border.setOutlineThickness(3.0f);
	set_font(font);
	hold = false;

	this->But_act = But_act;
	text = sf::Text(str, *font, size.y / 2);
	text.setFillColor(sf::Color::White);
	text.setOutlineColor(sf::Color::Black);
	text.setOutlineThickness(1.0f);

	stabilize_text();
}

i_soup::Button::Button(i_soup::b_context context)
{
	type = 0;
	this->obj = context.obj;
	border = sf::RectangleShape(context.size);
	border.setPosition(context.pos);
	border.setFillColor(sf::Color(150, 150, 150, 100));
	border.setOutlineColor(sf::Color(180, 180, 180, 210));
	border.setOutlineThickness(3.0f);
	set_font(context.font);
	hold = false;

	this->But_act = context.But_act;
	text = sf::Text(context.title, *context.font, context.size.y / 2);
	text.setFillColor(sf::Color::White);
	text.setOutlineColor(sf::Color::Black);
	text.setOutlineThickness(1.0f);

	stabilize_text();
}

void i_soup::Button::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
	target.draw(border, states);
	target.draw(text);
}

void i_soup::Button::click() {
	hold = true;
	change_state();
	if (But_act == 0) return;
	(obj->*But_act)();
}

void i_soup::Button::set_size(float size)
{
	return;
}

void i_soup::Button::input_char(char c)
{
	return;
}

void i_soup::Button::update()
{
}

bool i_soup::Button::is_hit(sf::Vector2f vect) {
	sf::FloatRect bounds = border.getGlobalBounds();
	if ((vect.x > bounds.left) && (vect.x < bounds.width + bounds.left) && (vect.y > bounds.top) && (vect.y < bounds.height + bounds.top)) {
		click();
		return true;
	}
	return false;
}

void i_soup::Button::set_font(sf::Font *font)
{
	text.setFont(*font);
	
	text.setCharacterSize(border.getSize().y / 2);
	text.setPosition(sf::Vector2f(border.getPosition().x + border.getSize().x / 2 - text.getLocalBounds().width/2, border.getPosition().y + border.getSize().y / 2 - text.getGlobalBounds().height));
}

void i_soup::Button::change_state()
{
	if (hold) {
		border.setFillColor(sf::Color(160, 160, 190, 50));
		border.setOutlineColor(sf::Color(180, 180, 180, 150));
	}
	else {
		border.setFillColor(sf::Color(150, 150, 150, 100));
		border.setOutlineColor(sf::Color(180, 180, 180, 210));
	}
}

void i_soup::Button::release()
{
	hold = false;
	change_state();
}

void i_soup::Button::set_text(std::string str)
{
	text.setString(sf::String(str));
	stabilize_text();
}

void i_soup::Button::set_action(action act)
{
	But_act = act;
}

void i_soup::Button::set_position(sf::Vector2f pos)
{
	border.setPosition(pos);
	stabilize_text();
}

void i_soup::Button::set_size(sf::Vector2f size)
{
	border.setSize(size);
	stabilize_text();
}

void i_soup::Button::stabilize_text() {
	text.setCharacterSize(border.getSize().y / 2);
	text.setPosition(sf::Vector2f(border.getPosition().x + border.getSize().x / 2 - text.getLocalBounds().width / 2, border.getPosition().y + border.getSize().y / 2 - text.getGlobalBounds().height));
}









i_soup::TextBox::TextBox()
{
	type = 1;
	is_int = 0;
	value = 1.0f;
	is_active = false;
	title = sf::Text();
	content = sf::Text();
	border = sf::RectangleShape();
	control_val = NULL;
}

i_soup::TextBox::TextBox(i_soup::tb_context context)
{
	type = 1;
	is_int = context.is_int;
	value = context.value;
	is_active = false;

	title = sf::Text(context.title, *context.font, context.global_size.y * 0.9f);
	title.setColor(sf::Color::White);
	title.setPosition(context.pos);
	

	border = sf::RectangleShape(sf::Vector2f(context.global_size.x - context.offset, context.global_size.y));
	border.setPosition(sf::Vector2f(context.offset + context.pos.x, context.pos.y));
	border.setFillColor(sf::Color(120, 120, 120, 100));
	border.setOutlineColor(sf::Color(255, 255, 255, 200));
	border.setOutlineThickness(2.0f);

	str = sf::String(std::to_string(value));
	str.resize(4);
	content = sf::Text(str, *context.font, border.getGlobalBounds().height * 0.6f);
	content.setColor(sf::Color(255, 255, 250, 255));

	set_control_val(context.control_val);

	stabilize_text();
}

i_soup::TextBox::TextBox(std::string title, float val, const sf::Font * font, sf::Vector2f global_size, sf::Vector2f pos, float offset)
{
	type = 1;
	is_int = 0;
	value = val;
	is_active = false;

	this->title = sf::Text(title, *font, global_size.y * 0.9f);
	this->title.setColor(sf::Color::White);
	this->title.setPosition(pos);

	border = sf::RectangleShape(sf::Vector2f(global_size.x - this->title.getGlobalBounds().width - 5, global_size.y));
	border.setPosition(sf::Vector2f(offset, pos.y));
	border.setFillColor(sf::Color(120, 120, 120, 100));
	border.setOutlineColor(sf::Color(255, 255, 255, 200));
	border.setOutlineThickness(2.0f);

	str = sf::String(std::to_string(val));
	str.resize(4);
	content = sf::Text(str, *font, border.getGlobalBounds().height * 0.6f);
	content.setColor(sf::Color(255, 255, 250, 255));

	
	control_val = NULL;

	stabilize_text();
}

void i_soup::TextBox::set_position(sf::Vector2f pos)
{
	sf::Vector2f offset = pos - border.getPosition();

	border.setPosition(pos + border.getPosition());
	title.setPosition(pos - title.getPosition());
	content.setPosition(pos - content.getPosition());
}

void i_soup::TextBox::set_size(float size)
{
	return;
}

void i_soup::TextBox::input_char(char c)
{
	if ((c == 59) && (!content.getString().isEmpty())) {
		str.pop_back();
		content.setString(str);
		return;
	}
	if (str.size() >= 4) return;
	if ((c >= 26) && (c < 36)) str.push_back((int)c + 22);
	if (c == 50) str.push_back('.');
	content.setString(str);

}

void i_soup::TextBox::update()
{
	if (is_int == 0)
		value = *control_val;
	if (is_int == 1)
		value = (float)*(unsigned int *)control_val;

	str = sf::String(std::to_string(value));
	str.resize(4);
	content.setString(str);
	return;
}

void i_soup::TextBox::set_control_val(float * val)
{
	control_val = val;
	change_control_val();
}

bool i_soup::TextBox::is_hit(sf::Vector2f vect)
{
	sf::FloatRect bounds = border.getGlobalBounds();
	if ((vect.x > bounds.left) && (vect.x < bounds.width + bounds.left) && (vect.y > bounds.top) && (vect.y < bounds.height + bounds.top)) {
		click();
		return true;
	}
	return false;
}

void i_soup::TextBox::set_is_int(int is_int)
{
	this->is_int = is_int;
}

void i_soup::TextBox::click()
{
	is_active = true;
	border.setOutlineColor(sf::Color(200, 130, 120, 220));
}

void i_soup::TextBox::change_control_val() {
	if (control_val == NULL) return;
	if (content.getString().isEmpty()) {
		str = sf::String(std::to_string(value));
		str.resize(4);
		content.setString(str);
		return;
	}
	try {
		value = std::stof(content.getString().toAnsiString());
	}
	catch (const std::invalid_argument) {
		str = sf::String(std::to_string(value));
		str.resize(4);
		content.setString(str);
		return;
	}
	if (is_int == 0) 
		*control_val = value;
	if (is_int == 1) 
		*(int*)control_val = value;
	
	str = sf::String(std::to_string(value));
	str.resize(4);
	content.setString(str);
}

void i_soup::TextBox::stabilize_text() {
	//content.setCharacterSize(border.getSize().y / 2);
	content.setPosition(sf::Vector2f(border.getPosition().x + 3, border.getPosition().y + border.getSize().y / 2 - content.getGlobalBounds().height * 0.7));
}

void i_soup::TextBox::release()
{
	is_active = false;
	this->border.setOutlineColor(sf::Color(255, 255, 255, 200));
	change_control_val();
}

void i_soup::TextBox::set_title(std::string new_title)
{
	title.setString(new_title);
}

void i_soup::TextBox::set_val(float new_val)
{
	value = new_val;
	str = sf::String(std::to_string(value));
	str.resize(4);
	content.setString(str);
}

void i_soup::TextBox::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
	target.draw(border);
	target.draw(title);
	target.draw(content);
	//if (is_int == 0) *control_val = value;
	//if (is_int == 1) *(int*)control_val = value;
}

void i_soup::InterfaceObject::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
	return;
}
