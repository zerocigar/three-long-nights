#include <cstdlib>
#include <iostream>
#include "Animals.h"
#include "Player.h"
#include "GameHelpers.h"

int stepToward(const int& from, const int& to)
{
	if (from > to) return -1;
	else if (to > from) return 1;
	else return 0;
}

Animal::Animal(int startingX, int startingY, char ascii, int startingHP, int foodValue) :
	x(startingX), y(startingY), symbol(ascii), HP(startingHP), foodValue(foodValue)  { }

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

bool Animal::isAlive() const
{
	return alive;
}

int Animal::getFoodValue() const
{
	return foodValue;
}

void Animal::assignRunAwayTiles()
{
	runAwayTiles = rand() % 5 + 1;
}

bool Animal::move(int newX, int newY, const std::vector<Tile>& world, const std::vector<Animal*>& animals)
{
	if (canEnter(newX, newY, world) && !otherAnimalInTile(*this, newX, newY, animals))
	{
		x = newX;
		y = newY;
		return true;
	}

	return false;
}

void Animal::takeDamage()
{
	HP -= 1;

	if (HP <= 0)
	{
		alive = false;
		HP = 0;
	}

	beingAttacked = true;
	assignRunAwayTiles();
}


Bear::Bear(int startingX, int startingY) :
	homeX(startingX), homeY(startingY), Animal(startingX, startingY, 'B', 10, 10)
{ };

bool Bear::isInTerritory(int px, int py) const
{
	return manhattanDistance(px, py, homeX, homeY) <= TERRITORY_RADIUS;
}

void Bear::chase(const std::vector<Tile>& world, Player& player, const std::vector<Animal*>& animals)
{
	int dx{ stepToward(x, player.getX()) };
	int dy{ stepToward(y, player.getY()) };

	if (dx && dy) // diagonal check
	{
		if (rand() % 2 == 0)
		{
			if (!move(x, y + dy, world, animals)) // try vertical movement first
				move(x + dx, y, world, animals);
		}
		else
		{
			if (!move(x + dx, y, world, animals)) // try horizontal movement first
				move(x, y + dy, world, animals);
		}
	}
	else if (dx) // if horizontal difference only
	{
		if (!move(x + dx, y, world, animals))
			move(x, y + dy, world, animals);
	}
	else if (dy) // if vertical difference only
	{
		if (!move(x, y + dy, world, animals))
			move(x + dx, y, world, animals);
	}

	if (!isInTerritory(player.getX(), player.getY()) && aggroTiles > 0)
		aggroTiles--;
}

void Bear::wanderTerritory(const std::vector<Tile>& world, Player& player, const std::vector<Animal*>& animals)
{
	// if not in territory
	if (!isInTerritory(x, y))
	{
		int dx{ stepToward(x, homeX) };
		int dy{ stepToward(y, homeY) };

		if (dx)
		{
			if (!move(x + dx, y, world, animals))
				move(x, y + dy, world, animals);
		}
		else
			move(x, y + dy, world, animals);

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

	move(x + randomX, y + randomY, world, animals);
}

std::vector<int> Bear::getNearestResourceTile(std::vector<Tile>& world, int tick) const
{
	bool found{ false };
	int nearestDistance{ WIDTH * HEIGHT };
	std::vector<int> coords{0, 0};

	for (int y{ 0 }; y < HEIGHT; y++)
	{
		for (int x{ 0 }; x < WIDTH; x++)
		{
			if ((manhattanDistance(x, y, Bear::x, Bear::y) < nearestDistance) && world[cellIndex(x, y)].getHasResource(tick) && world[cellIndex(x, y)].getTileType() != Tile::Grass)
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

void Bear::forage(std::vector<Tile>& world, Player& player, int tick, const std::vector<Animal*>& animals)
{

	if (manhattanDistance(x, y, player.getX(), player.getY()) <= 1)
	{
		chase(world, player, animals);
		return;
	}

	std::vector<int> coords{ getNearestResourceTile(world, tick) };

	if (tilesToForage == 0 || coords.empty())
	{
		wanderTerritory(world, player, animals);
		return;
	}

	int dx{ stepToward(x, coords[0]) };
	int dy{ stepToward(y, coords[1]) };

	if (dx)
	{
		if (!move(x + dx, y, world, animals))
			move(x, y + dy, world, animals);
	}
	else
		move(x, y + dy, world, animals);

	if (manhattanDistance(x, y, coords[0], coords[1]) <= 1)
	{
		world[cellIndex(coords[0], coords[1])].takeResource(tick);
		tilesToForage--;
	}
}

void Bear::takeTurn(std::vector<Tile>& world, Player& player, bool isDay, int tick, const std::vector<Animal*>& animals)
{
	if (wasDay && !isDay)
		tilesToForage = rand() % 6 + 3;
	wasDay = isDay;

	if (isInTerritory(player.getX(), player.getY()) && !aggroTiles && isDay)
		aggroTiles = rand() % 6 + 5;

	if (aggroTiles)
		chase(world, player, animals);
	else if (isDay)
		wanderTerritory(world, player, animals);
	else
		forage(world, player, tick, animals);

	if (x == player.getX() && y == player.getY())
	{
		int damageTaken{ rand() % 2 + 6 };
		player.takeDamage(damageTaken);
		std::cout << "The bear mauls you, dealing " << damageTaken << " damage!\n";

		if (aggroTiles > 0 && !isInTerritory(player.getX(), player.getY()))
			if (rand() % 100 + 1 <= 75)
				aggroTiles = 0;
	}
}

Chicken::Chicken(int startingX, int startingY) :
	Animal(startingX, startingY, 'c', 2, 2) { }

void Chicken::takeTurn(std::vector<Tile>& world, Player& player, bool isDay, int tick, const std::vector<Animal*>& animals)
{
	if (beingAttacked)
	{
		std::vector<int> xOffsets{ -1, 1, 0, 0 };
		std::vector<int> yOffsets{ 0, 0, -1, 1 };

		int currentDistance{ manhattanDistance(x, y, player.getX(), player.getY()) };
		int bestDistance{ currentDistance };
		int bestX{ x };
		int bestY{ y };
		bool escapeFound{ false };

		for (int i{ 0 }; i < 4; i++)
		{
			int candidateX{ x + xOffsets[i] };
			int candidateY{ y + yOffsets[i] };

			if (!canEnter(candidateX, candidateY, world))
				continue;

			int candidateDistance{ manhattanDistance(candidateX, candidateY, player.getX(), player.getY()) };

			if (candidateDistance > bestDistance)
			{
				bestDistance = candidateDistance;
				bestX = candidateX;
				bestY = candidateY;
				escapeFound = true;
			}
		}

		if (escapeFound)
			move(bestX, bestY, world, animals);

		runAwayTiles--;
	}
	else
	{
		int randomXDirection{ rand() % 3 - 1 };
		int randomYDirection{ rand() % 3 - 1 };
		int moveXorY{ rand() % 2 };

		if (moveXorY == 0)
		{
			if (!move(x + randomXDirection, y, world, animals))
				move(x, y + randomYDirection, world, animals);
		}
		else
		{
			if (!move(x, y + randomYDirection, world, animals))
				move(x + randomXDirection, y, world, animals);
		}

	}

	if (runAwayTiles <= 0)
		beingAttacked = false;
}

Boar::Boar(int startingX, int startingY, Bear& bear) :
	bear(bear), Animal(startingX, startingY, 'P', 4, 4) { }

void Boar::takeTurn(std::vector<Tile>& world, Player& player, bool isDay, int tick, const std::vector<Animal*>& animals)
{
	if (beingAttacked && aggroTiles <= 0)
		aggroTiles = rand() % 4 + 3;

	if (aggroTiles)
	{
		chase(world, player, animals);
		if (x == player.getX() && y == player.getY())
		{
			player.takeDamage(1);
			std::cout << "The boar attacks you with its horns!\n";
		}
	}
	else
		roam(world, animals);
}

void Boar::chase(const std::vector<Tile>& world, Player& player, const std::vector<Animal*>& animals)
{
	std::vector<int> xOffsets{ -1, 1, 0, 0 };
	std::vector<int> yOffsets{ 0, 0, -1, 1 };

	int currentDistance{ manhattanDistance(x, y, player.getX(), player.getY()) };
	int bestDistance{ currentDistance };
	int bestX{ x };
	int bestY{ y };
	bool routeFound{ false };

	for (int i{ 0 }; i < 4; i++)
	{
		int candidateX{ x + xOffsets[i] };
		int candidateY{ y + yOffsets[i] };

		if (!canEnter(candidateX, candidateY, world))
			continue;

		int candidateDistance{ manhattanDistance(candidateX, candidateY, player.getX(), player.getY()) };

		if (candidateDistance < bestDistance)
		{
			bestDistance = candidateDistance;
			bestX = candidateX;
			bestY = candidateY;
			routeFound = true;
		}
	}

	if (routeFound)
		move(bestX, bestY, world, animals);


	aggroTiles--;

	if (aggroTiles <= 0)
		beingAttacked = false;
}

void Boar::roam(const std::vector<Tile>& world, const std::vector<Animal*>& animals)
{
	std::vector<int> xOffsets{ -1, 1, 0, 0 };
	std::vector<int> yOffsets{ 0, 0, -1, 1 };

	std::vector<int> candidateMovesX{};
	std::vector<int> candidateMovesY{};

	for (int i{ 0 }; i < 4; i++)
	{
		if (canEnter(x + xOffsets[i], y + yOffsets[i], world))
		{
			if (manhattanDistance(x + xOffsets[i], y + yOffsets[i], bear.getX(), bear.getY()) <= 2 && bear.isAlive())
				continue;

			candidateMovesX.push_back(xOffsets[i]);
			candidateMovesY.push_back(yOffsets[i]);
		}
	}

	// if no valid moves
	if (candidateMovesX.empty())
		return;

	std::size_t randomMove{ rand() % candidateMovesX.size()};

	move(x + candidateMovesX[randomMove], y + candidateMovesY[randomMove], world, animals);
}