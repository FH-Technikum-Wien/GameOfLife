#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

#include "Lib/Timing.h"

#define ALIVE 'x'
#define DEAD '.'

static const bool RULE_TABLE[2][9]{ {0,0,0,1,0,0,0,0,0},{0,0,1,1,0,0,0,0,0} };

enum class ArgumentType
{
	LOAD, SAVE, GENERATIONS, MEASURE, HELP
};

std::map<std::string, ArgumentType> argumentMap{
	{"--load", ArgumentType::LOAD},
	{"--save", ArgumentType::SAVE},
	{"--generations", ArgumentType::GENERATIONS},
	{"--measure", ArgumentType::MEASURE},
	{"--measure", ArgumentType::HELP},
};

std::string InputFile = "random10000_in.gol";
std::string OutputFile = "out.gol";

unsigned int Generations = 250;
bool ShowMeasurements = true;

bool** world;
bool** temp;
bool** newWorld;


void processArgs(int argc, char* argv[]);
void showWrongArgs();
void showHelp();

bool** runGenerations(unsigned int width, unsigned int height, unsigned int generations);
int getNeighborsAlive(bool** world, int x, int y, unsigned int width, unsigned int height);

int main(int argc, char* argv[])
{
	// Handle arguments.
	processArgs(argc, argv);

	Timing* time = Timing::getInstance();

	//----------------------------------------------------------------------------------------------------
	// SETUP
	//----------------------------------------------------------------------------------------------------
	time->startSetup();

	// Read file.
	std::ifstream input;
	input.open(InputFile);
	if (!input.is_open())
	{
		std::cerr << "COULD NOT OPEN INPUT-FILE. WRONG PATH?" << std::endl;
		return 1;
	}

	// Get board size.
	std::string columns, rows;
	std::getline(input, columns, ',');
	std::getline(input, rows);
	unsigned int width = std::stoi(columns);
	unsigned int height = std::stoi(rows);
	unsigned int worldSize = width * height;

	// Create arrays
	world = new bool* [height];
	newWorld = new bool* [height];
	for (unsigned int i = 0; i < height; i++)
	{
		world[i] = new bool[width];
		newWorld[i] = new bool[width];
	}

	// Convert file to bool-array.
	for (unsigned int y = 0; y < height; y++)
	{
		std::string line;
		std::getline(input, line);
		for (unsigned int x = 0; x < width; x++)
		{
			world[y][x] = line[x] == ALIVE;
		}
	}
	input.close();

	time->stopSetup();
	//----------------------------------------------------------------------------------------------------
	// Calculation
	//----------------------------------------------------------------------------------------------------
	time->startComputation();

	// Go through all generations.
	bool** result = runGenerations(width, height, Generations);

	time->stopComputation();
	//----------------------------------------------------------------------------------------------------
	// Output
	//----------------------------------------------------------------------------------------------------
	time->startFinalization();

	// Write result to file.
	std::ofstream output;
	output.open(OutputFile, std::ios::out | std::ios::trunc);
	output << width << "," << height << std::endl;
	for (unsigned int y = 0; y < height; y++)
	{
		for (unsigned int x = 0; x < width; x++)
		{
			char alive = result[y][x] ? ALIVE : DEAD;
			output << alive;
		}
		output << std::endl;
	}
	output.close();
	time->stopFinalization();

	if (showHelp)
		time->print(true);

	return 0;
}


void processArgs(int argc, char* argv[])
{
	// Skip first argument (it's the .exe).
	for (unsigned int i = 1; i < argc; i++)
	{
		std::string argType = std::string(argv[i]);
		std::string argData = i + 1 < argc ? std::string(argv[i + 1]) : std::string();

		switch (argumentMap[argType])
		{
		case ArgumentType::LOAD:
			if (argData.empty())
				showWrongArgs();
			InputFile = argData;
			i++;
			break;
		case ArgumentType::SAVE:
			if (argData.empty())
				showWrongArgs();
			OutputFile = argData;
			i++;
			break;
		case ArgumentType::GENERATIONS:
			if (argData.empty())
				showWrongArgs();
			Generations = std::stoi(argData);
			i++;
			break;
		case ArgumentType::MEASURE:
			ShowMeasurements = true;
			break;
		case ArgumentType::HELP:
			showHelp();
			std::exit(0);
			break;
		default:
			showWrongArgs();
			break;
		}
	}
}

void showWrongArgs()
{
	std::cerr << "Wrong arguments. Use arguments as following:" << std::endl;
	showHelp();
	std::exit(1);
}

void showHelp()
{
	std::cout << "--load <file> \t\t\t-> Filename to read from." << std::endl;
	std::cout << "--save <file> \t\t\t-> Filename to save to." << std::endl;
	std::cout << "--generations <number of generations> \t-> Number of generations to go through." << std::endl;
	std::cout << "--measure \t\t\t-> Whether to print out time measurements." << std::endl;
}

bool** runGenerations(unsigned int width, unsigned int height, unsigned int generations)
{
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

int getNeighborsAlive(bool** world, int x, int y, unsigned int width, unsigned int height)
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