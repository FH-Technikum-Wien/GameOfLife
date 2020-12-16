__kernel void process_cell(__global const bool* world, __global bool* newWorld, __global const int* dimensions) {
    bool RULE_TABLE[2][9] = { {0,0,0,1,0,0,0,0,0},{0,0,1,1,0,0,0,0,0} };

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

/*
    bool topLeft = ID - width - 1;
    bool top = ID - width;
    bool topRight = ID - width + 1;
    bool left = ID - 1;
    bool right = ID + 1;
    bool bottomLeft = ID + width - 1;
    bool bottom = ID + width;
    bool bottomRight = ID + width + 1;

    // TODO: WRAPPING -> VERY SLOW!

    // Upper-Left corner
    if(x == 0 && y == 0){
        topLeft = width * height - 1;
    }
    // Upper-Right corner
    if(x == width - 1 && y == 0){
        topRight = width * (height - 1);
    }
    // Lower-Left corner
    if(x == 0 && y == height - 1){
        bottomRight = width - 1;
    }
    // Lower-Right corner
    if(x == width - 1 && y == height - 1){
        bottomRight = 0;
    }

    // Top row
    if(y == 0){
        top = width * (height - 1) + x;
    }
    // Bottom row
    if(y == height - 1){
        bottom = x;
    }
    // Left column
    if(x == 0){
        left = y * width + width - 1;
    }
    // Right column
    if(x == width - 1){
        right = y * width;
    }
    
    // Get neighbors
    int neighborsAlive = world[topLeft] + world[top] + world[topRight] + world[left] + world[right] + world[bottomLeft] + world[bottom] + world[bottomRight];
    */