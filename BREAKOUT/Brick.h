#pragma once
#include <SFML/Graphics.hpp>

class Brick
{
public:
	bool enable = true;
	sf::RectangleShape picture;
	void initiate();
	void setSize(float width, float height);
	void setPosition(float x, float y);
	virtual void hit();
};

