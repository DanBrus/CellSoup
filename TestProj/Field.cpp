#include "Field.h"
#include <SFML/Graphics.hpp>


Field::Field(int rows, int cols, sf::Vector2u box) 
{
	this->rows = rows;
	this->cols = cols;
	//������������ �������
	float offers[4], a;

	offers[0] = box.x / 100;
	offers[1] = box.y / 70;
	offers[2] = box.x - offers[0];
	offers[3] = box.y - offers[1];
	a = (offers[2] - offers[0]) / cols;
	if (((offers[3] - offers[1]) / a * 0.866) < rows) a = (offers[3] - offers[1]) / rows / 0.866;
	if ((offers[0] - a) < 0) {
		offers[0] += a / 2;
		offers[2] += a / 2;
	}
	if ((offers[1] - a / 2) < 0) {
		offers[1] += a / 2;
		offers[3] += a / 2;
	}

	net = Borders(offers, a, rows, cols);

	field = Tiles(rows, cols, net.get_tiles());
	//����������� ���������� �������.
	//����� ������� �� triangle
}

int Field::change_tile_color(unsigned int R, unsigned int G, unsigned int B, unsigned long long ctr)
{
	return field.change_tile_color(R, G, B, ctr);
}

int Field::change_tile_color(unsigned int R, unsigned int G, unsigned int B, unsigned int alpha, unsigned int row, unsigned int col)
{
	unsigned int ctr = row * cols + col;
	return field.change_tile_color(R, G, B, alpha, ctr);
}
int Field::change_tile_color(unsigned int R, unsigned int G, unsigned int B, unsigned int row, unsigned int col)
{
	unsigned int ctr = row * cols + col;
	return field.change_tile_color(R, G, B, ctr);
}

sf::Vector2f Field::get_size() {
	return field.get_size();
}

Field::Borders::Borders(float * offers, float a, int rows, int cols)
{
	m_vertices.setPrimitiveType(sf::Lines);
	m_vertices.resize(rows * cols * 12);

	calculate_tailmap(rows, cols, sf::Vector2f(offers[0], offers[1]), a);

}

void Field::Borders::calculate_tailmap(int rows, int cols, sf::Vector2f start, float a)
{
	float borderX = start.x;
	bool offer = false;
	for (int row = rows - 1; row >= 0; row--) {
		for (int col = cols - 1; col >= 0; col--) {
			Hexagon(start.x, start.y, a / 1.73, &m_vertices[(row * cols + col) * 12]);
			start.x += a;
		}
		start.x = borderX + a / 2;
		if (offer) start.x -= a / 2;
		offer = !offer;
		start.y += a * 0.866;
	}
}

void Field::Borders::Hexagon(float coreX, float coreY, float r, sf::Vertex * vertices)
{
	vertices[0].position = sf::Vector2f(coreX, coreY + r);
	vertices[1].position = sf::Vector2f(coreX + r * 0.9, coreY + r / 2);
	vertices[2].position = sf::Vector2f(coreX + r * 0.9, coreY + r / 2);
	vertices[3].position = sf::Vector2f(coreX + r * 0.9, coreY - r / 2);
	vertices[4].position = sf::Vector2f(coreX + r * 0.9, coreY - r / 2);
	vertices[5].position = sf::Vector2f(coreX, coreY - r);
	vertices[6].position = sf::Vector2f(coreX, coreY - r);
	vertices[7].position = sf::Vector2f(coreX - r * 0.9, coreY - r / 2);
	vertices[8].position = sf::Vector2f(coreX - r * 0.9, coreY - r / 2);
	vertices[9].position = sf::Vector2f(coreX - r * 0.9, coreY + r / 2);
	vertices[10].position = sf::Vector2f(coreX - r * 0.9, coreY + r / 2);
	vertices[11].position = sf::Vector2f(coreX, coreY + r);

	tile cur;
	cur.centre = sf::Vector2f(coreX, coreY);
	cur.verteces.push_back(vertices[0].position = sf::Vector2f(coreX, coreY + r));
	cur.verteces.push_back(vertices[1].position = sf::Vector2f(coreX + r * 0.9, coreY + r / 2));
	cur.verteces.push_back(vertices[3].position = sf::Vector2f(coreX + r * 0.9, coreY - r / 2));
	cur.verteces.push_back(vertices[5].position = sf::Vector2f(coreX, coreY - r));
	cur.verteces.push_back(vertices[7].position = sf::Vector2f(coreX - r * 0.9, coreY - r / 2));
	cur.verteces.push_back(vertices[9].position = sf::Vector2f(coreX - r * 0.9, coreY + r / 2));
	tiles.push_back(cur);
}

Field::Tiles::Tiles(int rows, int cols, std::vector<tile>* tiles_)
{
	tile cur;
	m_vertices.setPrimitiveType(sf::Triangles);

	for (int i = tiles_[0].size() - 1; i >= 0; i--) {
		cur = tiles_[0][i];
		tiles.push_back(cur);
	}

	fill_vertices();
}

void Field::Tiles::fill_vertices()
{
	m_vertices.resize(tiles.size() * 12);

	for (unsigned int i = 0; i < tiles.size(); i++) {
		sf::Vertex *vertices = &m_vertices[i * 12];

		vertices[0].position = tiles[i].verteces[0];
		vertices[1].position = tiles[i].verteces[1];
		vertices[2].position = tiles[i].verteces[2];
		vertices[3].position = tiles[i].verteces[2];
		vertices[4].position = tiles[i].verteces[3];
		vertices[5].position = tiles[i].verteces[4];
		vertices[6].position = tiles[i].verteces[4];
		vertices[7].position = tiles[i].verteces[5];
		vertices[8].position = tiles[i].verteces[0];
		vertices[9].position = tiles[i].verteces[0];
		vertices[10].position = tiles[i].verteces[2];
		vertices[11].position = tiles[i].verteces[4];

		change_tile_color(50, 50, 50, i);
	}

	//update_color();
}

int Field::Tiles::change_tile_color(unsigned int R, unsigned int G, unsigned int B, unsigned int ctr)
{
	if (ctr >= tiles.size()) return 1;
	if ((R > 255) || (G > 255) || (B > 255)) return 2;

	if (tiles[ctr].change_color(R, G, B)) return 3;

	update_color(ctr);
	return 0;
}

int Field::Tiles::change_tile_color(unsigned int R, unsigned int G, unsigned int B, unsigned int alpha, unsigned int ctr)
{
	if (ctr >= tiles.size()) return 1;
	if ((R > 255) || (G > 255) || (B > 255)) return 2;

	if (tiles[ctr].change_color(R, G, B, alpha)) return 3;

	update_color(ctr);
	return 0;
}

void Field::Tiles::update_color()
{
	for (int i = 0; i < tiles.size() * 12; i++) {
		m_vertices[i].color = tiles[i % 12].color;
	}
}

void Field::Tiles::update_color(unsigned int ctr)
{
	for (int i = 0; i < 12; i++) {
		m_vertices[ctr * 12 + i].color = tiles[ctr].color;
	}
}

sf::Vector2f Field::Tiles::get_size() {
	sf::Vector2f size(0, 0);
	for each (tile hexagon in tiles) {
		if (hexagon.verteces[1].x > size.x) size.x = hexagon.verteces[1].x;
		if (hexagon.verteces[0].y > size.y) size.y = hexagon.verteces[0].y;
	}

	size.x++;
	size.y++;
	return size;
}