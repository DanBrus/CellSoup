#pragma once
#include <SFML/Graphics.hpp>

class Field
{
	typedef struct tile_ {
		int count;
		sf::Color color;
		sf::Vector2f centre;
		std::vector<sf::Vector2f> verteces;


		int change_color(unsigned int R, unsigned int G, unsigned int B, unsigned int alpha = 255) {
			color = sf::Color(R, G, B, alpha);
			return 0;
		}
	}tile;

	class Borders : public sf::Drawable, public sf::Transformable
	{
	public:
		Borders() {	}

		Borders(float *offers, float a, int rows, int cols);
		void calculate_tailmap(int rows, int cols, sf::Vector2f start, float a);
		void Hexagon(float coreX, float coreY, float r, sf::Vertex *vertices);
		void change_field_size(int rows, int cols, float *offers);
		std::vector<tile> *get_tiles() {
			return &tiles;
		}


	private:

		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
		{
			// apply the entity's transform -- combine it with the one that was passed by the caller
			states.transform *= getTransform(); // getTransform() is defined by sf::Transformable

												// apply the texture
			states.texture = &m_texture;

			// you may also override states.shader or states.blendMode if you want

			// draw the vertex array
			target.draw(m_vertices, states);
		}

		sf::VertexArray m_vertices;
		sf::Texture m_texture;
		std::vector<tile> tiles;
	};

	class Tiles : public sf::Drawable, public sf::Transformable
	{
	private:
		sf::VertexArray m_vertices;
		sf::Texture m_texture;
		std::vector<tile> tiles;
	public:
		Tiles() {}
		Tiles(int rows, int cols, std::vector<tile> *tiles_);
		void change_field_size(int rows, int cols, std::vector<tile>* tiles_);
		void fill_vertices();
		int change_tile_color(unsigned int R, unsigned int G, unsigned int B, unsigned int alpha, unsigned int ctr);
		int change_tile_color(unsigned int R, unsigned int G, unsigned int B, unsigned int ctr);
		sf::Vector2f get_size();
		int is_hit(sf::Vector2f pos, int rows, int cols);

	private:
		void update_color();

		void update_color(unsigned int ctr);

		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
		{
			// apply the entity's transform -- combine it with the one that was passed by the caller
			states.transform *= getTransform(); // getTransform() is defined by sf::Transformable

												// apply the texture
			states.texture = &m_texture;

			// you may also override states.shader or states.blendMode if you want

			// draw the vertex array
			target.draw(m_vertices, states);
		}

	};

private:
	Borders net;
	Tiles field;
	unsigned int rows, cols;
	float offers[4];

public:
	Field(){}

	Field(int rows, int cols, sf::Vector2u box);

	Borders *get_borders() {		//Функция отвечает за получение Borders для отрисовки
		return &net;
	}

	Tiles *get_tiles() {		//Функция отвечает за получение Tiles для отрисовки
		return &field;
	}

	void change_size(int rows, int cols);

	sf::Vector2f get_size();

	int change_tile_color(unsigned int R, unsigned int G, unsigned int B, unsigned long long ctr);
	int change_tile_color(unsigned int R, unsigned int G, unsigned int B, unsigned  int alpha, unsigned int row, unsigned int col);
	int change_tile_color(unsigned int R, unsigned int G, unsigned int B, unsigned int row, unsigned int col);
	int is_hit(sf::Vector2f pos);

};


