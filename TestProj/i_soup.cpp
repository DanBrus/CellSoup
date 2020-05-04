#include "i_soup.h"
#include "CellSoup.h"
#include <Windows.h>


i_soup::i_soup(CellSoup *Core, sf::Vector2i window_size, sf::Vector2f size, int rows, int cols)
{
	//Filling variables
	this->Core = Core;
	mouse_hold = false;
	cur_zoom = 1;
	this->window_size = window_size;
	mouse_pos = sf::Vector2f(0, 0);
	activeButton == NULL;

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
	
	//Making buttons
	buttons.resize(6);
	fonts.push_back(sf::Font());
	fonts[0].loadFromFile("PetscopWide.ttf");
	for (int i = 0; i < 5; i++) {
		Button tmp(sf::Vector2f(30, 10 + i * (35)), sf::Vector2f(90.0f, 30.0f), "TEST", NULL, &fonts[0], this);
		buttons[i] = tmp;
	}
	buttons[0].set_text("Start");
	buttons[0].set_action(&i_soup::start_simulation);
	buttons[1].set_text("Stop");
	buttons[1].set_action(&i_soup::stop_simulation);
	buttons[2].set_text("Make life");
	buttons[2].set_action(&i_soup::make_life);
	buttons[3].set_text("Clean field");
	buttons[3].set_action(&i_soup::clean_field);
	buttons[4].set_text("Change mode");
	buttons[4].set_action(&i_soup::change_mode);

}

i_soup::~i_soup()
{
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
	if (mouse_hold) {
		sf::Vector2f offset((mouse_pos.x - x) * field_view.getSize().x / ((float)window_size.x * 0.83f), (mouse_pos.y - y) * field_view.getSize().y / ((float)window_size.y * 0.85f));
		move_field(offset);
	}

	mouse_pos.x = x;
	mouse_pos.y = y;
}

void i_soup::l_click()
{
	is_controls(mouse_pos);
	if (is_field(mouse_pos)) {
		mouse_hold = true;
	}
	if (is_controls(mouse_pos)) {
		for (int i = 0; i < buttons.size(); i++) {
			if (buttons[i].is_hit(global_to_local(mouse_pos, controls_view))) activeButton = &buttons[i];
		}
	}
}

void i_soup::l_release()
{
	mouse_hold = false;
	if (activeButton != NULL) activeButton->release();
	activeButton = NULL;
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
	Core->is_run = true;
}

void i_soup::stop_simulation()
{
	Core->is_run = false;
}

void i_soup::make_life()
{
	Core->is_run = false;
	Sleep(50);
	Core->free_field = false;
	Sleep(50);
	Core->b_make_life = true;
}

void i_soup::clean_field()
{
	Core->is_run = false;
	Sleep(50);
	Core->free_field = true;
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

	sf::RectangleShape control_rect(sf::Vector2f(window_size.x * 0.14f, window_size.y * 0.85f));

	control_rect.setFillColor(sf::Color(120, 120, 45, 90));
	control_rect.setOutlineThickness(-5.f);
	control_rect.setOutlineColor(sf::Color(30, 90, 255, 255));

	sf::RectangleShape field_border(sf::Vector2f(window_size.x * 0.83f, window_size.y * 0.85f));
	field_border.setFillColor(sf::Color(0, 0, 0, 0));
	field_border.setPosition(window_size.x * 0.01f, window_size.y * 0.02f);
	field_border.setOutlineColor(sf::Color(255, 255, 255, 255));
	field_border.setOutlineThickness(5.f);

	target.setView(main_view);
	target.draw(field_border);

	target.setView(info_view);
	target.draw(info_rect);

	target.setView(controls_view);
	//target.draw(control_rect);
	
	for(int i = 0; i < buttons.size(); i++)
		target.draw(buttons[i]);

	target.setView(field_view);
}














i_soup::Button::Button() {
	border.setFillColor(sf::Color(150, 150, 150, 100));
	border.setOutlineColor(sf::Color(180, 180, 180, 210));
	border.setOutlineThickness(3.0f);
	hold = false;
}

i_soup::Button::Button(sf::Vector2f pos, sf::Vector2f size, std::string str, action But_act, sf::Font *font, i_soup *obj)
{
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