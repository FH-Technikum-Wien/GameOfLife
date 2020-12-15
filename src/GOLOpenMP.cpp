#include "GOLOpenMP.h"
#include <omp.h>
#include <iostream>

constexpr bool RULE_TABLE[2][9]{ {0,0,0,1,0,0,0,0,0},{0,0,1,1,0,0,0,0,0} };

bool** GOLOpenMP::runGenerations(bool** world, bool** newWorld, int width, int height, int generations)
{
	omp_set_dynamic(1);
	omp_set_num_threads(11);
	bool** temp;
	for (int i = 0; i < generations; i++)
	{
		// Enter parallel section
#pragma omp parallel
		{
			int neighborsAlive = 0;
			// Share iterations. Allow continuation. 
# pragma omp for nowait
			for (int x = 0; x < width; x++)
			{
				neighborsAlive = getNeighborsAlive(world, x, 0, width, height);
				newWorld[0][x] = RULE_TABLE[world[0][x]][neighborsAlive];
				neighborsAlive = getNeighborsAlive(world, x, height - 1, width, height);
				newWorld[height - 1][x] = RULE_TABLE[world[height - 1][x]][neighborsAlive];
			}
			// Share iterations. Allow continuation. 
# pragma omp for nowait
			for (int y = 1; y < height - 1; y++)
			{
				neighborsAlive = getNeighborsAlive(world, 0, y, width, height);
				newWorld[y][0] = RULE_TABLE[world[y][0]][neighborsAlive];
				neighborsAlive = getNeighborsAlive(world, width - 1, y, width, height);
				newWorld[y][width - 1] = RULE_TABLE[world[y][width - 1]][neighborsAlive];
			}

			// Share iterations. Allow continuation. 
# pragma omp for nowait
			for (int y = 1; y < height - 1; y++)
			{
				for (int x = 1; x < width - 1; x++)
				{
					neighborsAlive = world[y - 1][x - 1] + world[y - 1][x] + world[y - 1][x + 1] + world[y][x - 1] + world[y][x + 1] + world[y + 1][x - 1] + world[y + 1][x] + world[y + 1][x + 1];
					newWorld[y][x] = RULE_TABLE[world[y][x]][neighborsAlive];
				}
			}
		}
		temp = world;
		world = newWorld;
		newWorld = temp;
	}
	return world;
}

int GOLOpenMP::getNeighborsAlive(bool** world, int x, int y, int width, int height)
{
	int alive = 0;
# pragma omp parallel
	{
		# pragma omp for nowait
		for (int yOffset = -1; yOffset <= 1; yOffset++)
		{
			for (int xOffset = -1; xOffset <= 1; xOffset++)
			{
				// Skip self
				if (yOffset == 0 && xOffset == 0)
					continue;
				// Wrap around.
				int currentX = x + xOffset;
				currentX = currentX == width ? 0 : currentX < 0 ? width - 1 : currentX;
				int currentY = y + yOffset;
				currentY = currentY == height ? 0 : currentY < 0 ? height - 1 : currentY;

				if (world[currentY][currentX])
					alive++;
			}
		}
	}
	return alive;
}
