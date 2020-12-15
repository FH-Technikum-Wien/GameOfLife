#include "GOLSingleThread.h"

constexpr bool RULE_TABLE[2][9]{ {0,0,0,1,0,0,0,0,0},{0,0,1,1,0,0,0,0,0} };

bool** GOLSingleThread::runGenerations(bool** world, bool** newWorld, unsigned int width, unsigned int height, unsigned int generations)
{
	bool** temp;
	// Calculate inner field
	for (unsigned int i = 0; i < generations; i++)
	{
		// Calculate border
		// Top and bottom
		int neighborsAlive = 0;
		for (unsigned int x = 0; x < width; x++)
		{
			neighborsAlive = getNeighborsAlive(world, x, 0, width, height);
			newWorld[0][x] = RULE_TABLE[world[0][x]][neighborsAlive];
			neighborsAlive = getNeighborsAlive(world, x, height - 1, width, height);
			newWorld[height - 1][x] = RULE_TABLE[world[height - 1][x]][neighborsAlive];
		}
		// Left and right
		for (unsigned int y = 1; y < height - 1; y++)
		{
			neighborsAlive = getNeighborsAlive(world, 0, y, width, height);
			newWorld[y][0] = RULE_TABLE[world[y][0]][neighborsAlive];
			neighborsAlive = getNeighborsAlive(world, width - 1, y, width, height);
			newWorld[y][width - 1] = RULE_TABLE[world[y][width - 1]][neighborsAlive];
		}

		// Calculate inside
		for (unsigned int y = 1; y < height - 1; y++)
		{
			for (unsigned int x = 1; x < width - 1; x++)
			{
				// Check neighnors.
				neighborsAlive =
					world[y - 1][x - 1] +	// Top left
					world[y - 1][x] +		// Top middle 
					world[y - 1][x + 1] +	// Top right
					world[y][x - 1] +		// Left
					world[y][x + 1] +		// Right
					world[y + 1][x - 1] +	// Bottom left
					world[y + 1][x] +		// Bottom middle
					world[y + 1][x + 1];	// Bottom right

				newWorld[y][x] = RULE_TABLE[world[y][x]][neighborsAlive];
			}
		}
		temp = world;
		world = newWorld;
		newWorld = temp;
	}
	return world;
}

int GOLSingleThread::getNeighborsAlive(bool** world, int x, int y, unsigned int width, unsigned int height)
{
	unsigned int alive = 0;
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
	return alive;
}
