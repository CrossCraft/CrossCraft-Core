#include <CrossCraft/indev.h>

void CrossCraft_Indev_House(uint16_t x, uint16_t y, uint16_t z) {
    LevelMap* map = CrossCraft_World_GetMapPtr();

    // Set Floor & Ceiling
    for(int xn = x - 3; xn <= x + 3; xn++) {
        for(int zn = z - 3; zn <= z + 3; zn++) {
            if(BoundCheckMap(map, xn, y, zn) && BoundCheckMap(map, xn, y + 4, zn)) {
                SetBlockInMap(map, xn, y, zn, 48);    // Mossy cobble
                SetBlockInMap(map, xn, y + 1, zn, 0);
                SetBlockInMap(map, xn, y + 2, zn, 0);
                SetBlockInMap(map, xn, y + 3, zn, 0);
                SetBlockInMap(map, xn, y + 4, zn, 48);// Mossy cobble
            }
        }
    }

    for(int xn = x - 3; xn <= x + 3; xn++) {
        int zn = z - 3;
        if(BoundCheckMap(map, xn, y + 1, zn) && BoundCheckMap(map, xn, y + 3, zn)) {
            SetBlockInMap(map, xn, y + 1, zn, 48);
            SetBlockInMap(map, xn, y + 2, zn, 48);
            SetBlockInMap(map, xn, y + 3, zn, 48);
        }
    }


    for(int xn = x - 3; xn <= x + 3; xn++) {
        int zn = z + 3;
        if(BoundCheckMap(map, xn, y + 1, zn) && BoundCheckMap(map, xn, y + 3, zn)) {
            SetBlockInMap(map, xn, y + 1, zn, 48);
            SetBlockInMap(map, xn, y + 2, zn, 48);
            SetBlockInMap(map, xn, y + 3, zn, 48);
        }
    }


    for(int zn = z - 3; zn <= z + 3; zn++) {
        int xn = x - 3;
        if(BoundCheckMap(map, xn, y + 1, zn) && BoundCheckMap(map, xn, y + 3, zn)) {
            SetBlockInMap(map, xn, y + 1, zn, 48);
            SetBlockInMap(map, xn, y + 2, zn, 48);
            SetBlockInMap(map, xn, y + 3, zn, 48);
        }
    }


    for(int zn = z - 3; zn <= z + 3; zn++) {
        int xn = x + 3;
        if(BoundCheckMap(map, xn, y + 1, zn) && BoundCheckMap(map, xn, y + 3, zn)) {
            SetBlockInMap(map, xn, y + 1, zn, 48);
            SetBlockInMap(map, xn, y + 2, zn, 48);
            SetBlockInMap(map, xn, y + 3, zn, 48);
        }
    }

    y += 1;

    SetBlockInMap(map, x + 3, y, z, 0);
    SetBlockInMap(map, x + 3, y + 1, z, 0);

    SetBlockInMap(map, x + 2, y, z + 2, 54);
    SetBlockInMap(map, x - 1, y, z + 2, 54);
    SetBlockInMap(map, x - 2, y, z + 2, 54);


    SetBlockInMap(map, x - 1, y, z - 2, 54);
    SetBlockInMap(map, x + 1, y, z - 2, 54);
}