#include "Player.h"
#include <iostream>

Player::Player(int startingX, int startingY, int startingHunger, int startingThirst) :
	x(startingX), y(startingY), hunger(startingHunger), thirst(startingThirst), symbol('@'), health(10)
{ };

int Player::getX() const
{
	return x;
}

int Player::getY() const
{
	return y;
}

char Player::getSymbol() const
{
	return symbol;
}

int Player::getHealth() const
{
	return health;
}

int Player::getHunger() const
{
	return hunger;
}

int Player::getThirst() const
{
	return thirst;
}

void Player::setPos(int newX, int newY)
{
	x = newX;
	y = newY;
}

void Player::interactTile(Tile& tile, int tick)
{
	//TODO: check if there's an entity at the tile BEFORE checking the tile type, entity interaction comes first

	if (tile.getTileType() == Tile::Water)
	{
		if (tile.getHasResource(tick))
		{
			std::cout << "You take a few sips from the water before you.\n";
			drink();
			tile.takeResource(tick);
		}
		else
		{
			std::cout << "There's hardly any water to drink. It'll have to refill.\n";
		}
	}
	else if (tile.getTileType() == Tile::Bush)
	{
		if (tile.getHasResource(tick))
		{
			std::cout << "You take some berries from the bush and eat them.\n";
			eat(1);
			tile.takeResource(tick);
		}
		else
		{
			std::cout << "The bush is bare.\n";
		}
	}
	else
	{
		std::cout << "You feel the grass. The grass almost seems like it's feeling back.\n";
	}
}

void Player::takeDamage(int damage)
{
	health -= damage;

	if (health < 0)
		health = 0;
}

void Player::eat(int hungerValue)
{
	hunger += hungerValue;

	if (hunger > MAX_HUNGER)
		hunger = MAX_HUNGER;
}

void Player::drink()
{
	thirst += 1;

	if (thirst > MAX_THIRST)
		thirst = MAX_THIRST;
}

void Player::heal(int amount)
{
	health += amount;
	if (health > MAX_HEALTH)
		health = MAX_HEALTH;
}

void Player::getHungry()
{
	hunger -= 1;

	if (hunger < 0)
		hunger = 0;
}

void Player::getThirsty()
{
	thirst -= 1;

	if (thirst < 0)
		thirst = 0;
}