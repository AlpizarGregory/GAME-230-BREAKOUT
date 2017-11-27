#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>

#define _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC_NEW
#include <cstdlib>
#include <crtdbg.h>
#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif


#include "Paddle.h"
#include "Ball.h"
#include "Brick.h"

using namespace sf;
using namespace std;

const float pi = 3.14159f;

RenderWindow window;
RectangleShape background;
Texture textureBack;
Font font;
Text gameoverText;
Text lifeText;

Clock gameClock;
float deltaTime;

float frameWidth = 800;
float frameHeight = 800;

bool isPlaying = false;
bool gameover = false;
bool win = false;

int life = 3;
int level = 0;

Paddle paddle;
Ball ball;

vector<Brick*> bricks;

void Initiate();
void Reset();
void Update();
void Render();
void HandleInput();
void loadLevel(int level);

bool BallLeft(RectangleShape rect);
bool BallRight(RectangleShape rect);
bool BallUp(RectangleShape rect);
bool BallBottom(RectangleShape rect);

int main()
{
	
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF |
		_CRTDBG_LEAK_CHECK_DF);


	window.create(VideoMode(frameWidth, frameHeight), "BREAKOUT");
	Initiate();
	loadLevel(0);
	while (window.isOpen())
	{

		deltaTime = gameClock.restart().asSeconds();
		HandleInput();

		if (!isPlaying &&!gameover&&!win)
		{
			ball.picture.setPosition(paddle.picture.getPosition().x, paddle.picture.getPosition().y - paddle.picture.getSize().y / 2 - ball.picture.getRadius());
		}
		else if (!gameover&&!win)
		{
			Update();
		}



		Render();
	}

	return EXIT_SUCCESS;


}

void Initiate()
{
	font.loadFromFile("consola.ttf");


	lifeText.setFont(font);
	lifeText.setCharacterSize(20);
	lifeText.setPosition(20, 650);
	lifeText.setString("life:"+ std::to_string(life));

	gameoverText.setFont(font);
	gameoverText.setCharacterSize(35);
	gameoverText.setPosition(100, 400);
	gameoverText.setString("");

}

void Reset()
{
	ball.setPosition(paddle.picture.getPosition().x, paddle.picture.getPosition().y - paddle.picture.getSize().y / 2 - ball.picture.getRadius());
	ball.angle = (270 + std::rand() % 60 - 30) * 2 * pi / 360;

}

void Update()
{
	float factor = ball.speed * deltaTime;
	ball.picture.move(std::cos(ball.angle) * factor, std::sin(ball.angle) * factor);


	//physics

	//edge
	if (ball.picture.getPosition().y + ball.picture.getRadius() > frameHeight)
	{
		isPlaying = false;
		life--;
		Reset();
	}else if (ball.picture.getPosition().x - ball.picture.getRadius() < 0.f)
	{
		ball.angle = pi - ball.angle;
		ball.picture.setPosition(ball.picture.getRadius() + 0.1f, ball.picture.getPosition().y);
	}
	else if (ball.picture.getPosition().x + ball.picture.getRadius() > frameWidth)
	{
		ball.angle = pi - ball.angle;
		ball.setPosition(frameWidth - ball.picture.getRadius() - 0.1f, ball.picture.getPosition().y);
	}
	else if (ball.picture.getPosition().y - ball.picture.getRadius() < 0.f)
	{
		ball.angle = -ball.angle;
		ball.setPosition(ball.picture.getPosition().x, ball.picture.getRadius()+0.1f);
	}

	//paddle
	if (BallBottom(paddle.picture))
	{
		int dis = ball.picture.getPosition().x - paddle.picture.getPosition().x;
		ball.angle = -ball.angle;
		if (dis > 0 && ball.angle < 3.f / 2.f*pi)
		{
			ball.angle += (dis % 20) * pi / 180;
		}
		else if (dis < 0 && ball.angle > 3.f / 2.f*pi)
		{
			ball.angle -= (dis % 20) * pi / 180;
		}
		ball.setPosition(ball.picture.getPosition().x, paddle.picture.getPosition().y - paddle.picture.getSize().y/2 - ball.picture.getRadius() - 0.1f);
	}
	//bricks
	for (int i = 0; i < bricks.size(); ++i)
	{
		if (bricks[i]->enable)
		{
			if (BallUp(bricks[i]->picture))
			{
				bricks[i]->hit();
				ball.angle = -ball.angle;
				ball.setPosition(ball.picture.getPosition().x, bricks[i]->picture.getPosition().y + bricks[i]->picture.getSize().y / 2 + ball.picture.getRadius() + 0.1f);
			}
			else if (BallBottom(bricks[i]->picture))
			{
				bricks[i]->hit();
				ball.angle = -ball.angle;
				ball.setPosition(ball.picture.getPosition().x, bricks[i]->picture.getPosition().y - bricks[i]->picture.getSize().y / 2 - ball.picture.getRadius() - 0.1f);
			}
			else if (BallLeft(bricks[i]->picture))
			{
				bricks[i]->hit();
				ball.angle = pi-ball.angle;
				ball.setPosition(bricks[i]->picture.getPosition().x + ball.picture.getRadius() + bricks[i]->picture.getSize().x / 2 + 0.1f, ball.picture.getPosition().y);
			}
			else if (BallRight(bricks[i]->picture))
			{
				bricks[i]->hit();
				ball.angle = pi-ball.angle;
				ball.setPosition(bricks[i]->picture.getPosition().x - ball.picture.getRadius() - bricks[i]->picture.getSize().x / 2 - 0.1f, ball.picture.getPosition().y);
			}
		}
	}
	if (life <= 0)
	{
		gameover = true;
		gameoverText.setString("game over, press \"Enter\" restart");
	}
	int count = 0;
	for (int i = 0; i < bricks.size(); ++i)
	{
		if (bricks[i]->enable)
			count++;
	}

	if (count <= 0)
	{
		win = true;
		gameoverText.setString("win! press \"Enter\" to next");
	}
	lifeText.setString("life:" + std::to_string(life));
}

void Render()
{
	window.clear(sf::Color::Black);
	window.draw(paddle.picture);
	window.draw(ball.picture);

	for (int i = 0; i < bricks.size(); ++i)
	{
		if (bricks[i]->enable)
		{
			window.draw(bricks[i]->picture);
		}

	}
	window.draw(lifeText);
	window.draw(gameoverText);
	window.display();
}

void HandleInput()
{
	sf::Event event;
	while (window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			window.close();
			for (int i = 0; i < bricks.size(); ++i)
			{
				delete bricks[i];
				bricks[i] = nullptr;
			}
			bricks.clear();
		}
		else if (event.type == sf::Event::MouseMoved&&!gameover)
		{
			paddle.picture.setPosition(Vector2f(event.mouseMove.x,paddle.picture.getPosition().y));
		}
	}



	if (!gameover)
	{
		if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A)) &&
			(paddle.picture.getPosition().x - paddle.picture.getSize().x / 2.f > 5.f))
		{
			paddle.picture.move(-paddle.speed * deltaTime, 0.f);
		}
		if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D)) &&
			(paddle.picture.getPosition().x + paddle.picture.getSize().x / 2.f < frameWidth - 5.f))
		{
			paddle.picture.move(paddle.speed * deltaTime, 0.f);
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) || sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			isPlaying = true;
		}

	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return))
	{
		if (gameover)
		{
			life = 3;
			gameover = false;
			loadLevel(level);
		}
		else if (win)
		{
			win = false;
			level = (level + 1) % 3;
			loadLevel(level);
		}
	}
	
}


void loadLevel(int level)
{
	life = 3;

	isPlaying = false;
	gameover = false;


	gameoverText.setString("");

	paddle.initiate();
	paddle.setSize(150, 25);
	paddle.setPosition(frameWidth / 2, frameHeight - paddle.picture.getSize().y);

	ball.initiate();
	ball.setSize(15);
	ball.setPosition(paddle.picture.getPosition().x, paddle.picture.getPosition().y - paddle.picture.getSize().y / 2 - ball.picture.getRadius());
	ball.angle = (270 + std::rand() % 60 - 30) * 2 * pi / 360;


	for (int i = 0; i < bricks.size(); ++i)
	{
		delete bricks[i];
		bricks[i] = nullptr;
	}
	bricks.clear();

	if (level == 0)
	{
		
		for (int i = 0; i < 1; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				Brick* bptr = new Brick;
				bptr->initiate();
				bptr->setSize(100, 30);
				bptr->setPosition(bptr->picture.getSize().x / 2 + j*bptr->picture.getSize().x, bptr->picture.getSize().y / 2 + (i + 3)*bptr->picture.getSize().y);
				bricks.push_back(bptr);
			}
		}
	}
	else if (level == 1)
	{
		paddle.initiate();
		paddle.setSize(150, 25);
		paddle.setPosition(frameWidth / 2, frameHeight - paddle.picture.getSize().y);

		ball.initiate();
		ball.setSize(15);
		ball.setPosition(paddle.picture.getPosition().x, paddle.picture.getPosition().y - paddle.picture.getSize().y / 2 - ball.picture.getRadius());
		ball.angle = (270 + std::rand() % 60 - 30) * 2 * pi / 360;

	}
	else if (level == 2)
	{
		paddle.initiate();
		paddle.setSize(150, 25);
		paddle.setPosition(frameWidth / 2, frameHeight - paddle.picture.getSize().y);

		ball.initiate();
		ball.setSize(15);
		ball.setPosition(paddle.picture.getPosition().x, paddle.picture.getPosition().y - paddle.picture.getSize().y / 2 - ball.picture.getRadius());
		ball.angle = (270 + std::rand() % 60 - 30) * 2 * pi / 360;

	}
}

bool BallLeft(RectangleShape rect)
{
	if (ball.picture.getPosition().x + ball.picture.getRadius() > rect.getPosition().x - rect.getSize().x / 2 &&
		ball.picture.getPosition().x + ball.picture.getRadius() < rect.getPosition().x + rect.getSize().x / 2 &&
		ball.picture.getPosition().y + ball.picture.getRadius() >= rect.getPosition().y - rect.getSize().y / 2 &&
		ball.picture.getPosition().y - ball.picture.getRadius() <= rect.getPosition().y + rect.getSize().y / 2)
		return true;
	else
		return false;
}
bool BallRight(RectangleShape rect)
{
	if (ball.picture.getPosition().x - ball.picture.getRadius() > rect.getPosition().x - rect.getSize().x / 2 &&
		ball.picture.getPosition().x - ball.picture.getRadius() < rect.getPosition().x + rect.getSize().x / 2 &&
		ball.picture.getPosition().y + ball.picture.getRadius() >= rect.getPosition().y - rect.getSize().y / 2 &&
		ball.picture.getPosition().y - ball.picture.getRadius() <= rect.getPosition().y + rect.getSize().y / 2)
		return true;
	else
		return false;
}
bool BallUp(RectangleShape rect)
{
	if (ball.picture.getPosition().x + ball.picture.getRadius() >= rect.getPosition().x - rect.getSize().x / 2 &&
		ball.picture.getPosition().x - ball.picture.getRadius() <= rect.getPosition().x + rect.getSize().x / 2 &&
		ball.picture.getPosition().y - ball.picture.getRadius() < rect.getPosition().y + rect.getSize().y / 2 &&
		ball.picture.getPosition().y - ball.picture.getRadius() > rect.getPosition().y - rect.getSize().y / 2)
		return true;
	else
		return false;
}
bool BallBottom(RectangleShape rect)
{
	if (ball.picture.getPosition().x + ball.picture.getRadius() >= rect.getPosition().x - rect.getSize().x / 2 &&
		ball.picture.getPosition().x - ball.picture.getRadius() <= rect.getPosition().x + rect.getSize().x / 2 &&
		ball.picture.getPosition().y + ball.picture.getRadius() < rect.getPosition().y + rect.getSize().y / 2 &&
		ball.picture.getPosition().y + ball.picture.getRadius() > rect.getPosition().y - rect.getSize().y / 2)
		return true;
	else
		return false;
}
