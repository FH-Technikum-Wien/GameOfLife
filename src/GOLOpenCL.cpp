#include "GOLOpenCL.h"
#include <CL/cl.hpp>

#include <iostream>
#include <fstream>


constexpr bool RULE_TABLE[2][9]{ {0,0,0,1,0,0,0,0,0},{0,0,1,1,0,0,0,0,0} };

cl::Program program;
cl::CommandQueue queue;

cl::Buffer buffer_World;
cl::Buffer buffer_NewWorld;
cl::Buffer buffer_Dimensions;

size_t worldSize;

bool* world1D;
bool* newWorld1D;

void GOLOpenCL::setup(bool** world, bool** newWorld, int width, int height)
{
	// Convert 2D arrays to 1D
	unsigned int size = width * height;
	world1D = new bool[size];
	newWorld1D = new bool[size];
	for (unsigned int y = 0; y < height; y++)
	{
		unsigned int row = y * width;
		for (unsigned int x = 0; x < width; x++)
		{
			world1D[row + x] = world[y][x];
		}
	}

	// Get all platforms (drivers), e.g. NVIDIA
	std::vector<cl::Platform> all_platforms;
	cl::Platform::get(&all_platforms);

	if (all_platforms.size() == 0)
	{
		std::cerr << " No platforms found. Check OpenCL installation!" << std::endl;
		std::exit(1);
	}
	cl::Platform default_platform = all_platforms[0];

	// Get default device (CPUs, GPUs) of the default platform
	std::vector<cl::Device> all_devices;
	default_platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);
	if (all_devices.size() == 0)
	{
		std::cerr << " No devices found. Check OpenCL installation!" << std::endl;
		std::exit(1);
	}

	// Use device[0] because that's the GPU
	cl::Device default_device = all_devices[0];

	// A context is like a "runtime link" to the device and platform;
	// i.e. communication is possible
	cl::Context context({ default_device });

	// Create the program that we want to execute on the device
	cl::Program::Sources sources;

	// Covert kernel to string
	std::ifstream kernelFile("kernel/gol_kernel.cl");
	std::string kernelCode((std::istreambuf_iterator<char>(kernelFile)), (std::istreambuf_iterator<char>()));

	sources.push_back({ kernelCode.c_str(), kernelCode.length() });

	// Build kernel program
	program = cl::Program(context, sources);
	if (program.build({ default_device }) != CL_SUCCESS)
	{
		std::cerr << "Error building: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(default_device) << std::endl;
		exit(1);
	}

	// Convert game dimensions to array
	int dimensions[2]{ width, height };
	worldSize = size;

	// Allocate space on GPU
	buffer_World = cl::Buffer(context, CL_MEM_READ_ONLY, sizeof(bool) * worldSize);
	buffer_NewWorld = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(bool) * worldSize);
	buffer_Dimensions = cl::Buffer(context, CL_MEM_READ_ONLY, sizeof(int) * 2);

	// Create a queue (a queue of commands that the GPU will execute)
	queue = cl::CommandQueue(context, default_device);

	// Push write buffers to queue
	queue.enqueueWriteBuffer(buffer_World, CL_TRUE, 0, sizeof(bool) * worldSize, world1D);
	queue.enqueueWriteBuffer(buffer_NewWorld, CL_TRUE, 0, sizeof(bool) * worldSize, newWorld1D);
	queue.enqueueWriteBuffer(buffer_Dimensions, CL_TRUE, 0, sizeof(int) * 2, dimensions);
}

bool** GOLOpenCL::runGenerations(bool** world, bool** newWorld, int width, int height, int generations)
{
	for (unsigned int i = 0; i < generations; i++)
	{	
		// Send data and execute
		cl::Kernel process_cell(program, "process_cell");
		process_cell.setArg(0, buffer_World);
		process_cell.setArg(1, buffer_NewWorld);
		process_cell.setArg(2, buffer_Dimensions);
		queue.enqueueNDRangeKernel(process_cell, cl::NullRange, cl::NDRange(worldSize));
		queue.finish();

		// Copy newWorld to world for repetition
		queue.enqueueCopyBuffer(buffer_NewWorld, buffer_World, 0, 0, sizeof(bool) * worldSize);
	}
	// Read result from GPU to here
	queue.enqueueReadBuffer(buffer_NewWorld, CL_TRUE, 0, sizeof(bool) * worldSize, newWorld1D);
	// Convert to 2D array
	for (unsigned int i = 0; i < worldSize; i++)
	{
		int x = i % width;
		int y = i / width;
		newWorld[y][x] = newWorld1D[i];
	}

	return newWorld;
}
