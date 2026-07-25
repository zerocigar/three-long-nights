#include <cstdlib>
#include <iostream>
#include "Animals.h"
#include "GameHelpers.h"

int stepToward(const int& from, const int& to)
{
	if (from > to) return -1;
	else if (to > from) return 1;
	else return 0;
}

Animal::Animal(int startingX, int startingY, char ascii) :
	x(startingX), y(startingY), symbol(ascii) { }

char Animal::getSymbol() const
{
	return symbol;
}

int Animal::getX() const
{
	return x;
}

int Animal::getY() const
{
	return y;
}

bool Animal::move(int newX, int newY, const std::vector<Tile>& world)
{
	if (canEnter(newX, newY, world))
	{
		x = newX;
		y = newY;
		return true;
	}

	return false;
}


Bear::Bear(int startingX, int startingY) :
	Animal(startingX, startingY, 'B')
{ };

void Bear::takeTurn(const std::vector<Tile>& world, Player& player)
{
	int dx{ stepToward(x, player.getX()) };
	int dy{ stepToward(y, player.getY()) };

	if (dx && dy) // diagonal check
	{
		if (rand() % 2 == 0)
		{
			if (!move(x, y + dy, world)) // try vertical movement first
				move(x + dx, y, world);
		}
		else
		{
			if (!move(x + dx, y, world)) // try horizontal movement first
				move(x, y + dy, world);
		}
	}
	else if (dx) // if horizontal difference only
	{
		if (!move(x + dx, y, world))
			move(x, y + dy, world);
	}
	else if (dy) // if vertical difference only
	{
		if (!move(x, y + dy, world))
			move(x + dx, y, world);
	}

	if (x == player.getX() && y == player.getY())
	{
		int damageTaken{ rand() % 2 + 6 };
		player.takeDamage(damageTaken);
		std::cout << "The bear mauls you, dealing " << damageTaken << " damage!\n";
	}
}