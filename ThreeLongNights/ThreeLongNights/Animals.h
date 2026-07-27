#ifndef ANIMALS08475718
#define ANIMALS08475718
#include "Tile.h"
#include "Player.h"
#include <vector>

class Animal
{
protected:
	char symbol;
	int x;
	int y;

public:
	char getSymbol() const;
	int getX() const;
	int getY() const;
	virtual void takeTurn(std::vector<Tile>& world, Player& player, bool isDay, int tick) = 0;
	bool move(int x, int y, const std::vector<Tile>& world);
	Animal(int startingX, int startingY, char ascii);
};


class Bear : public Animal
{
	int homeX;
	int homeY;
	int aggroTiles{ 0 };
	int tilesToForage{ 0 };
	bool wasDay{ true };
	static constexpr int TERRITORY_RADIUS{ 2 };
public:
	void takeTurn(std::vector<Tile>& world, Player& player, bool isDay, int tick) override;
	void chase(const std::vector<Tile>& world, Player& player);
	void wanderTerritory(const std::vector<Tile>& world, Player& player);
	void forage(std::vector<Tile>& world, Player& player, int tick);
	bool isInTerritory(int newX, int newY) const;
	std::vector<int> getNearestResourceTile(const std::vector<Tile>& world) const;
	Bear(int startingX, int startingY);
};

#endif