#ifndef GAMEHELPERS9575676719
#define GAMEHELPERS9575676719

#include <vector>
#include <string>
#include "Tile.h"
#include "Player.h"

// establish width and height of grid
constexpr inline int WIDTH{ 10 };
constexpr inline int HEIGHT{ 10 };

bool isActionValid(const std::vector<char>& actions, char input);
char getValidatedPlayerAction(const std::string& prompt, const std::vector<char>& actions);
int cellIndex(int xCoord, int yCoord);
std::vector<Tile> createWorld();
bool isDay(int tick);
bool isInBounds(int x, int y);
bool canEnter(int x, int y, const std::vector<Tile>& world);
void applyOffsets(char input, int& x, int& y);
std::vector<int> getValidStartingCoordinates(const std::vector<Tile>& world, const Player& player);
int manhattanDistance(int x1, int y1, int x2, int y2);

#endif