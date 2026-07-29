#ifndef PLAYER5837191
#define PLAYER5837191

#include "Tile.h"

class Animal;

class Player {
	int x;
	int y;
	int health;
	int hunger;
	int thirst;
	char symbol;
	const int MAX_HUNGER{ 10 };
	const int MAX_THIRST{ 10 };
	const int MAX_HEALTH{ 10 };

public:
	int getX() const;
	int getY() const;
	char getSymbol() const;
	int getHealth() const;
	int getHunger() const;
	int getThirst() const;
	void setPos(int newX, int newY);
	void interactEntity(Animal& animal);
	void interactTile(Tile& tile, int tick);
	void takeDamage(int damage);
	void eat(int hungerValue);
	void getHungry();
	void getThirsty();
	void drink();
	void heal(int amount);
	Player(int startingX, int startingY, int startingHunger, int startingThirst);
};

#endif