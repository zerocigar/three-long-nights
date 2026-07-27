#include "Tile.h"

Tile::Tile(TileType type) :
	tileType(type), hasResource(true), lastTickThatHadResource(0)
{ 
	if (type == Tile::Water)
		symbol = '~';
	else if (type == Tile::Bush)
		symbol = 'x';
	else
		symbol = '.';
}

char Tile::getSymbol() const
{
	return symbol;
}

Tile::TileType Tile::getTileType() const
{
	return tileType;
}

bool Tile::getHasResource(int tick)
{
	if (tick >= lastTickThatHadResource + REPLENISH_TICKS)
		respawnResource();

	return hasResource;
}

void Tile::takeResource(int tick)
{
	hasResource = false;
	lastTickThatHadResource = tick;
}

void Tile::respawnResource()
{
	hasResource = true;
}