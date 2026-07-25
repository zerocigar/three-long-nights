#include <iostream>
#include <cstdlib>
#include <limits>
#include "GameHelpers.h"

bool isActionValid(const std::vector<char>& actions, char input)
{
	bool found{ false };

	for (char action : actions)
	{
		if (input == action)
		{
			found = true;
			break;
		}
	}

	return found;
}

// char input validation
char getValidatedPlayerAction(const std::string& prompt, const std::vector<char>& actions)
{
	char input{};

	while (true)
	{
		std::cout << prompt;
		std::cin >> input;

		if (std::cin.fail())
		{
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cout << "Invalid input.\n";
			continue;
		}

		bool found{ isActionValid(actions, input) };

		if (!found)
		{
			std::cout << "Invalid character. Please choose from the ones selected.\n";
			continue;
		}

		return input;
	}
}

// translation between x, y coords and grid index
int cellIndex(int xCoord, int yCoord)
{
	return yCoord * WIDTH + xCoord;
}

// main world generation
std::vector<Tile> createWorld()
{
	std::vector<Tile> world{};

	for (int i{ 0 }; i < WIDTH * HEIGHT; i++)
	{
		int rng = rand() % 100 + 1;

		if (rng <= 5)
			world.push_back(Tile{ Tile::Water });
		else if (rng <= 90)
			world.push_back(Tile{ Tile::Grass });
		else
			world.push_back(Tile{ Tile::Bush });
	}

	return world;
}

bool isDay(int tick)
{
	return ((tick - 1) % 100 < 50);
}

// check if tile even exists
bool isInBounds(int x, int y)
{
	return x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT;
}

// check if the grid tile it's trying to reach is accessible (not water)
bool canEnter(int x, int y, const std::vector<Tile>& world)
{
	return isInBounds(x, y)
		&& world[cellIndex(x, y)].getTileType() != Tile::Water;
}

// logic moved from main
// takes input and applies properly
void applyOffsets(char input, int& x, int& y)
{
	if (input == 'W' || input == 'w')
		y -= 1;
	else if (input == 'A' || input == 'a')
		x -= 1;
	else if (input == 'S' || input == 's')
		y += 1;
	else if (input == 'D' || input == 'd')
		x += 1;
}

std::vector<int> getValidStartingCoordinates(const std::vector<Tile>& world, const Player& player)
{
	int randomX{ rand() % WIDTH };
	int randomY{ rand() % HEIGHT };

	int distance{ abs(randomX - player.getX()) + abs(randomY - player.getY()) };
	while (!canEnter(randomX, randomY, world) || distance <= 3)
	{
		randomX = rand() % WIDTH;
		randomY = rand() % HEIGHT;
		distance = abs(randomX - player.getX()) + abs(randomY - player.getY());
	}

	return std::vector<int>{randomX, randomY};
}