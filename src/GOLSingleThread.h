#pragma once

class GOLSingleThread
{
public:
	static bool** runGenerations(bool** world, bool** newWorld, unsigned int width, unsigned int height, unsigned int generations);
	static int getNeighborsAlive(bool** world, int x, int y, unsigned int width, unsigned int height);
};

