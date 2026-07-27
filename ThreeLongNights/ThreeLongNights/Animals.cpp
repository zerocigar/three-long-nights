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
	homeX(startingX), homeY(startingY), Animal(startingX, startingY, 'B')
{ };

bool Bear::isInTerritory(int px, int py) const
{
	return manhattanDistance(px, py, homeX, homeY) <= TERRITORY_RADIUS;
}

void Bear::chase(const std::vector<Tile>& world, Player& player)
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

	if (!isInTerritory(player.getX(), player.getY()))
		aggroTiles--;
}

void Bear::wanderTerritory(const std::vector<Tile>& world, Player& player)
{
	// if not aggro'd and not in territory
	if (!aggroTiles && !isInTerritory(x, y))
	{
		int dx{ stepToward(x, homeX) };
		int dy{ stepToward(y, homeY) };

		if (dx)
		{
			if (!move(x + dx, y, world))
				move(x, y + dy, world);
		}
		else
			move(x, y + dy, world);

		return;
	}

	int randomX{ rand() % 3 - 1 };
	int randomY{ rand() % 3 - 1 };

	while (!canEnter(x + randomX, y + randomY, world) || !isInTerritory(x + randomX, y + randomY))
	{
		// if x is bad, change x; vice versa in else
		if (!canEnter(x + randomX, y, world) || !isInTerritory(x + randomX, y))
			randomX = rand() % 3 - 1;
		else
			randomY = rand() % 3 - 1;
	}

	move(x + randomX, y + randomY, world);
}

std::vector<int> Bear::getNearestResourceTile(const std::vector<Tile>& world) const
{
	bool found{ false };
	int nearestDistance{ WIDTH * HEIGHT };
	std::vector<int> coords{0, 0};

	for (int y{ 0 }; y < HEIGHT; y++)
	{
		for (int x{ 0 }; x < WIDTH; x++)
		{
			if ((manhattanDistance(x, y, Bear::x, Bear::y) < nearestDistance) && world[cellIndex(x, y)].getHasResource() && world[cellIndex(x, y)].getTileType() != Tile::Grass)
			{
				nearestDistance = manhattanDistance(x, y, Bear::x, Bear::y);
				coords[0] = x;
				coords[1] = y;
				found = true;
			}
		}
	}

	if (!found) return {};
	return coords;
}

void Bear::forage(std::vector<Tile>& world, Player& player, int tick)
{
	std::vector<int> coords{ getNearestResourceTile(world) };

	if (tilesToForage == 0 || coords.empty())
	{
		wanderTerritory(world, player);
		return;
	}

	int dx{ stepToward(x, coords[0]) };
	int dy{ stepToward(y, coords[1]) };

	if (dx)
	{
		if (!move(x + dx, y, world))
			move(x, y + dy, world);
	}
	else
		move(x, y + dy, world);

	if (manhattanDistance(x, y, coords[0], coords[1]) <= 1)
	{
		world[cellIndex(coords[0], coords[1])].takeResource(tick);
		tilesToForage--;
	}
}

void Bear::takeTurn(std::vector<Tile>& world, Player& player, bool isDay, int tick)
{
	if (wasDay && !isDay)
		tilesToForage = rand() % 6 + 3;
	wasDay = isDay;

	if (isInTerritory(player.getX(), player.getY()) && !aggroTiles && isDay)
		aggroTiles = rand() % 6 + 5;

	if (aggroTiles)
		chase(world, player);
	else if (isDay)
		wanderTerritory(world, player);
	else
		forage(world, player, tick);

	if (x == player.getX() && y == player.getY())
	{
		int damageTaken{ rand() % 2 + 6 };
		player.takeDamage(damageTaken);
		std::cout << "The bear mauls you, dealing " << damageTaken << " damage!\n";

		if (aggroTiles)
			if (rand() % 100 + 1 <= 75)
				aggroTiles = 0;
	}
}