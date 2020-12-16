#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

#include "util/Timing.h"
#include "src/GOLSingleThread.h"
#include "src/GOLOpenMP.h"
#include "src/GOLOpenCL.h"

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

std::string InputFile = "random10000_in.gol";
std::string OutputFile = "out.gol";

unsigned int Generations = 250;
bool ShowMeasurements = true;

void processArgs(int argc, char* argv[]);
void showWrongArgs();
void showHelp();

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
	bool** world = new bool* [height];
	bool** newWorld = new bool* [height];
	for (unsigned int i = 0; i < height; i++)
	{
		world[i] = new bool[width];
		newWorld[i] = new bool[width];
	}

	bool* world1D = new bool[worldSize];
	bool* newWorld1D = new bool[worldSize];

	// Convert file to bool-array.
	for (unsigned int y = 0; y < height; y++)
	{
		std::string line;
		std::getline(input, line);
		for (unsigned int x = 0; x < width; x++)
		{
			world[y][x] = line[x] == ALIVE;
			world1D[(y * width) + x] = line[x] == ALIVE;
		}
	}
	input.close();

	GOLOpenCL::setup(width, height);

	time->stopSetup();
	//----------------------------------------------------------------------------------------------------
	// Calculation
	//----------------------------------------------------------------------------------------------------
	time->startComputation();

	// Go through all generations.
	//bool** result = GOLSingleThread::runGenerations(world, newWorld, width, height, Generations);
	//bool** result = GOLOpenMP::runGenerations(world, newWorld, width, height, Generations);	
	bool* result1D = GOLOpenCL::runGenerations(world1D, newWorld1D, width, height, Generations);

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
			//char alive = result[y][x] ? ALIVE : DEAD;
			char alive = result1D[(y * width) + x] ? ALIVE : DEAD;
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