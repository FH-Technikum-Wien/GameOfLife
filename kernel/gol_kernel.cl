__constant bool RULE_TABLE[2][9] = { {0,0,0,1,0,0,0,0,0},{0,0,1,1,0,0,0,0,0} };

__kernel void process_cell(__global const bool* world, __global bool* newWorld, __global const int* dimensions) {

    int width = dimensions[0];
    int height = dimensions[1];

    // Get index of current element
    int ID = get_global_id(0);

    
    // Get indices
    int x = ID % width;
    int y = ID / width;

    unsigned int neighborsAlive = 0;
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

            if (world[(currentY * width) + currentX])
                neighborsAlive++;
        }
    }

    // Apply gol-rules
    newWorld[ID] = RULE_TABLE[world[ID]][neighborsAlive];
}