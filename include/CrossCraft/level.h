#pragma once
#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t count;
    EntityData* entities;
} LevelEntityList;

typedef struct {
    uint32_t count;
    TileEntityData* entities;
} LevelTileEntityList;

typedef struct {
    uint16_t width;
    uint16_t length;
    uint16_t height;

    uint16_t spawnX, spawnY, spawnZ;

    uint8_t* blocks;

    //block data << 4
    //light data & 0x0F
    uint8_t* data;
} LevelMap;

typedef struct {
    uint16_t timeOfDay;
    uint8_t skyBrightness;

    uint32_t skyColor;
    uint32_t fogColor;
    uint32_t cloudColor;
    uint16_t cloudHeight;

    uint8_t surroundingGroundType;
    uint16_t surroundingGroundHeight;

    uint8_t surroundingWaterType;
    uint16_t surroundingWaterHeight;
} LevelEnvironment;

typedef struct {
    uint64_t createdOn;
    const char* name;
    const char* author;
} LevelAbout;

typedef struct {
    LevelAbout about;
    LevelEnvironment environment;
    LevelMap map;
    LevelEntityList entities;
    LevelTileEntityList tileEntities;
} Level;

uint32_t GetPosFromXYZ(uint32_t x, uint32_t y, uint32_t z);

void GetXYZFromPos(uint32_t pos, uint32_t* x, uint32_t* y, uint32_t* z);

uint8_t GetDataFromMap(LevelMap* map, uint16_t x, uint16_t y, uint16_t z);

uint8_t GetBlockFromMap(LevelMap* map, uint16_t x, uint16_t y, uint16_t z);

void SetBlockInMap(LevelMap* map, uint16_t x, uint16_t y, uint16_t z, uint8_t block);

bool BoundCheckMap(LevelMap* map, uint16_t x, uint16_t y, uint16_t z);

#ifdef __cplusplus
};
#endif