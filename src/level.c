#include <CrossCraft/level.h>

uint32_t GetPosFromXYZ(uint32_t x, uint32_t y, uint32_t z) {
    return x + (y << 10) + (z << 20);
}

void GetXYZFromPos(uint32_t pos, uint32_t* x, uint32_t* y, uint32_t* z) {
    *x = pos % 1024;
    *y = (pos >> 10) % 1024;
    *z = (pos >> 20) % 1024;
}

uint8_t GetDataFromMap(LevelMap* map, uint16_t x, uint16_t y, uint16_t z) {
    uint32_t index = (y * map->length * map->width) + (z * map->width) + x;
    return map->data[index];
}

uint8_t GetBlockFromMap(LevelMap* map, uint16_t x, uint16_t y, uint16_t z) {
    uint32_t index = (y * map->length * map->width) + (z * map->width) + x;
    return map->blocks[index];
}

void SetBlockInMap(LevelMap* map, uint16_t x, uint16_t y, uint16_t z, uint8_t block) {
    uint32_t index = (y * map->length * map->width) + (z * map->width) + x;
    map->blocks[index] = block;
}

bool BoundCheckMap(LevelMap* map, uint16_t x, uint16_t y, uint16_t z) {
    return (x < map->length && y < map->height && z < map->width);
}
