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
	LOAD, SAVE, GENERATIONS, MEASURE, PRETTY, MODE, THREADS, HELP
};


std::map<std::string, ArgumentType> argumentMap{
	{"--load", ArgumentType::LOAD},
	{"--save", ArgumentType::SAVE},
	{"--generations", ArgumentType::GENERATIONS},
	{"--measure", ArgumentType::MEASURE},
	{"--pretty", ArgumentType::PRETTY},
	{"--mode", ArgumentType::MODE},
	{"--threads", ArgumentType::THREADS},
	{"--help", ArgumentType::HELP},
};

enum class ModeType
{
	SEQ, OMP, OCL
};


std::map<std::string, ModeType> modeMap{
	{"seq", ModeType::SEQ},
	{"omp", ModeType::OMP},
	{"ocl", ModeType::OCL},
};

std::map<ModeType, std::string> modeNameMap{
	{ModeType::SEQ, "Sequential"},
	{ModeType::OMP, "OpenMP"},
	{ModeType::OCL, "OpenCL"},
};

std::string InputFile = "random10000_in.gol";
std::string OutputFile = "out.gol";

unsigned int generations = 250;
bool showMeasurements = false;
bool prettyPrint = false;

ModeType mode = ModeType::SEQ;

unsigned int numberOfThreads = 0;

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

	bool** world = new bool* [height];
	bool** newWorld = new bool* [height];
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
			world[y][x] = line[x] == ALIVE;
	}
	input.close();

	if (mode == ModeType::OMP)
		GOLOpenMP::setup(numberOfThreads);

	if (mode == ModeType::OCL)
		GOLOpenCL::setup(world, newWorld, width, height);

	time->stopSetup();
	//----------------------------------------------------------------------------------------------------
	// Calculation
	//----------------------------------------------------------------------------------------------------
	time->startComputation();
	bool** result;
	// Go through all generations.

	switch (mode)
	{
	case ModeType::SEQ:
		result = GOLSingleThread::runGenerations(world, newWorld, width, height, generations);
		break;
	case ModeType::OMP:
		result = GOLOpenMP::runGenerations(world, newWorld, width, height, generations);
		break;
		break;
	case ModeType::OCL:
		result = GOLOpenCL::runGenerations(world, newWorld, width, height, generations);
		break;
	default:
		result = world;
		std::cerr << "ERROR! Couldn't execute requested mode";
		break;
	}

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

	if (prettyPrint)
		time->print(true);

	if (showMeasurements)
		std::cout << time->getResults() << std::endl;

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
			generations = std::stoi(argData);
			i++;
			break;
		case ArgumentType::MEASURE:
			showMeasurements = true;
			break;
		case ArgumentType::PRETTY:
			prettyPrint = true;
			break;
		case ArgumentType::MODE:
			if (argData.empty())
				showWrongArgs();
			mode = modeMap[argData];
			i++;
			break;
		case ArgumentType::THREADS:
			if (argData.empty())
				showWrongArgs();
			numberOfThreads = std::stoi(argData);
			i++;
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
	std::cout << std::endl;
	std::cout << "--load <file>                            -> Filename to read from." << std::endl;
	std::cout << "--save <file>                            -> Filename to save to." << std::endl;
	std::cout << "--generations <number of generations>    -> Number of generations to go through." << std::endl;
	std::cout << "--measure                                -> Whether to print out time measurements." << std::endl;
	std::cout << "--pretty                                 -> Whether to print out time pretty." << std::endl;
	std::cout << "--mode <modeType>                        -> Choose between: seq | omp | ocl" << std::endl;
	std::cout << "--threads <numberOfThreads>              -> Sets the threads used by OpenMP. If not set, uses all available." << std::endl;
	std::cout << "--help                                   -> Prints out this message." << std::endl;
}