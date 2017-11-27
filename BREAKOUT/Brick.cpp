#include "Brick.h"


void Brick::initiate()
{
	picture.setFillColor(sf::Color::White);
}
void Brick::setSize(float width, float height)
{
	picture.setSize(sf::Vector2f(width, height));
	picture.setOrigin(picture.getSize() / 2.f);
}
void Brick::setPosition(float x, float y)
{
	picture.setPosition(sf::Vector2f(x, y));
}
void Brick::hit()
{
	enable = false;
}