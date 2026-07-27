#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "Tile.h"
#include "Player.h"
#include "Animals.h"
#include "GameHelpers.h"

// prints the grid as passed to it with divisions between layers and labels the tick
void printWorld(const std::vector<Tile>& world, int tick, const Player& player, std::vector<Animal*>&animals)
{
	std::string timeOfDay { isDay(tick) ? "Day" : "Night"};

	std::cout << "Tick " << tick << " - " << timeOfDay << " | HP: " << player.getHealth() << "/10 | Hunger: " << player.getHunger() << "/10 | Thirst : " << player.getThirst() << "/10\n";
	// nested for loop to print rows (which come first, y) and columns (second, x)
	for (int y{ 0 }; y < HEIGHT; y++)
	{
		for (int x{ 0 }; x < WIDTH; x++)
		{
			bool animalFound{ false };
			for (Animal* animal : animals)
			{
				if (x == animal->getX() && y == animal->getY())
				{
					std::cout << ' ' << animal->getSymbol() << ' ';
					animalFound = true;
				}
			}

			if (!animalFound)
			{
				if (x == player.getX() && y == player.getY())
					std::cout << ' ' << player.getSymbol() << ' ';
				else
					std::cout << ' ' << world[cellIndex(x, y)].getSymbol() << ' ';
			}
		}

		std::cout << '\n';
	}
}

int getRandomHungerTickInterval()
{
	return rand() % 10 + 20;
}
int getRandomThirstTickInterval()
{
	return rand() % 7 + 13;
}


// main game loop
int main()
{
	srand(static_cast<unsigned>(time(nullptr)));

	int startingHunger{ rand() % 2 == 0 ? 7 : 8 };
	int startingThirst{ rand() % 2 == 0 ? 7 : 8 };
	Player player{ 2, 3, startingHunger, startingThirst };

	 
	int tick{ 1 };
	std::vector<char> validActionChars{ 'w', 'a', 's', 'd', 'x'};
	std::vector <Tile> world {createWorld()};

	int hungerTick{ getRandomHungerTickInterval()};
	int thirstTick{ getRandomThirstTickInterval()};

	std::vector<int> bearStartingCoords{ getValidStartingCoordinates(world, player) };
	Bear bear{ bearStartingCoords[0], bearStartingCoords[1]};

	std::vector<Animal*> animals{&bear};

	// game ends after 300 ticks (3 days)
	while (tick <= 300 && player.getHealth() > 0)
	{
		int newX{ player.getX()};
		int newY{ player.getY()};
		printWorld(world, tick, player, animals);

		char playerInput{ getValidatedPlayerAction("Type w/a/s/d for movement, or x to open the interact options.", validActionChars)};
		bool playerTriedToMove{ playerInput != 'x' && playerInput != 'X'};

		if (playerTriedToMove)
		{
			applyOffsets(playerInput, newX, newY);

			if (canEnter(newX, newY, world))
			{
				player.setPos(newX, newY);
			}
			else
			{
				std::cout << "You can't go that way.\n";
			}
		}
		else
		{
			playerInput = getValidatedPlayerAction("Type w/a/s/d respectively to interact with the tile in that direction, or type 'x' to interact with the tile you're in.\n", validActionChars);

			applyOffsets(playerInput, newX, newY);

			if (isInBounds(newX, newY))
			{
				player.interactTile(world[cellIndex(newX, newY)], tick);
			}
			else
			{
				std::cout << "There is no valid tile to interact with!\n";
			}
		}

		for (Animal* animal : animals)
		{
			animal->takeTurn(world, player, isDay(tick));
		}

		if (tick >= hungerTick)
		{
			player.getHungry();
			hungerTick = tick + getRandomHungerTickInterval();
		}

		if (tick >= thirstTick)
		{
			player.getThirsty();
			thirstTick = tick + getRandomThirstTickInterval();
		}

		if (player.getHunger() == 0 || player.getThirst() == 0)
		{
			if (player.getHunger() == 0)
			{
				std::cout << "You're starving...\n";
			}

			if (player.getThirst() == 0)
			{
				std::cout << "You're dying of dehydration...\n";
			}
			player.takeDamage(1);
		}

		tick++;
		std::cout << '\n';
	}

	if (player.getHealth() > 0)
	{
		std::cout << "You have survived three long nights.\n";
	}
	else
	{
		if (player.getX() == bear.getX() && player.getY() == bear.getY())
		{
			std::cout << "You were mauled to death.\n";
		}
		else if (player.getHunger() == 0 && player.getThirst() == 0)
		{
			std::cout << "You died of both starvation and dehydration. Malnourished to the highest degree.\n";
		}
		else if (player.getHunger() == 0)
		{
			std::cout << "You died of starvation.\n";
		}
		else if (player.getThirst() == 0)
		{
			std::cout << "You died of dehydration.\n";
		}
		else
		{
			std::cout << "The wilderness claimed you.\n";
		}
	}

	std::cin.get();
	return 0;

	// TODO: add game completion celebration
	// maybe with stats? original tiles explored, berries picked, etc?
}