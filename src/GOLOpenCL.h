#pragma once

class GOLOpenCL
{
public:
	static void setup(int width, int height);

	static bool* runGenerations(bool* world, bool* newWorld, int width, int height, int generations);
};

