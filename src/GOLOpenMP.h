#pragma once

class GOLOpenMP
{
public:
	static bool** runGenerations(bool** world, bool** newWorld, int width, int height, int generations);
	static int getNeighborsAlive(bool** world, int x, int y, int width, int height);
};

