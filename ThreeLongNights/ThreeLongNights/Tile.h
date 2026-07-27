#ifndef TILES0192949101
#define TILES0192949101

class Tile
{
	char symbol;
	bool hasResource;
	int lastTickThatHadResource;
	static constexpr int REPLENISH_TICKS{ 50 };
	void respawnResource();
public:
	enum TileType
	{
		Grass, Bush, Water
	};
private:
	TileType tileType;

public:
	char getSymbol() const;
	TileType getTileType() const;
	bool getHasResource(int tick);
	void takeResource(int tick);
	Tile(TileType type);
};

#endif