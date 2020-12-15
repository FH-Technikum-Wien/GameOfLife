#pragma once
static constexpr bool RULE_TABLE[2][9]{ {0,0,0,1,0,0,0,0,0},{0,0,1,1,0,0,0,0,0} };

class GOLOpenCL
{
public:
	static bool** runGenerations(bool** world, bool** newWorld, unsigned int width, unsigned int height, unsigned int generations);
	static int getNeighborsAlive(bool** world, int x, int y, unsigned int width, unsigned int height);
};

