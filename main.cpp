#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

#include "Lib/Timing.h"

#define ALIVE 'x'
#define DEAD '.'

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

std::string InputFile = "in.gol";
std::string OutputFile = "out.gol";
unsigned int Generations = 1;
bool ShowMeasurements = false;


void processArgs(int argc, char* argv[]);
void showWrongArgs();
void showHelp();

bool* runGenerations(bool* world, unsigned int width, unsigned int height, unsigned int generations);

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
	if (!input.is_open()) {
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

	// Convert file to bool-array.
	bool* world = new bool[worldSize];
	unsigned int index = 0;
	char cell;
	while (input.get(cell) && index < worldSize)
	{
		if (cell != ALIVE && cell != DEAD)
			continue;
		world[index] = cell == ALIVE;
		index++;
	}
	input.close();

	time->stopSetup();
	//----------------------------------------------------------------------------------------------------
	// Calculation
	//----------------------------------------------------------------------------------------------------
	time->startComputation();

	// Go through all generations.
	bool* result = runGenerations(world, width, height, Generations);

	time->stopComputation();
	//----------------------------------------------------------------------------------------------------
	// Output
	//----------------------------------------------------------------------------------------------------
	time->startFinalization();

	// Write result to file.
	std::ofstream output;
	output.open(OutputFile, std::ios::out | std::ios::trunc);
	output << width << "," << height << std::endl;
	for (unsigned int i = 0; i < worldSize; i++)
	{
		char alive = result[i] ? ALIVE : DEAD;
		output << alive;
		// New line after 'width' is reached.
		if ((i + 1) % width == 0 && i + 1 < worldSize)
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

bool* runGenerations(bool* world, unsigned int width, unsigned int height, unsigned int generations)
{
	unsigned int worldSize = width * height;
	bool* newWorld = new bool[worldSize];
	for (unsigned int i = 0; i < generations; i++)
	{
		for (unsigned int y = 0; y < height; y++)
		{
			for (unsigned int x = 0; x < width; x++)
			{
				unsigned int cellIndex = (y * width) + x;

				// Check neighbors.
				unsigned int numberOfAlive = 0;
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

						unsigned int currentNeighbor = (currentY * width) + currentX;

						if (world[currentNeighbor])
							numberOfAlive++;
					}
				}
				bool isAlive = world[cellIndex];
				// If three bring to live or stays alive.
				if (!isAlive && numberOfAlive == 3)
					newWorld[cellIndex] = true;
				// If less than two or more than 3 die
				else if ((isAlive && numberOfAlive < 2) || (isAlive && numberOfAlive > 3))
					newWorld[cellIndex] = false;
				else
					newWorld[cellIndex] = world[cellIndex];
			}
		}
		delete[] world;
		world = newWorld;
		newWorld = new bool[worldSize];
	}
	return world;
}