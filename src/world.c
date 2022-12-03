#include <CrossCraft/log.h>
#include <CrossCraft/world.h>
#include <CrossCraft/worldgen.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

static Level level;

void CrossCraft_World_Init() {
    CC_Internal_Log_Message(CC_LOG_DEBUG, "Generating base level template");
    srand(time(NULL));

    CrossCraft_WorldGenerator_Init(rand());

    LevelAbout abt = {
        .createdOn = time(NULL),
        .name = "A Nice World",
        .author = "Default"
    };

    LevelEnvironment env = {
            .timeOfDay = 1000,
            .skyBrightness = 11,
            .skyColor = 0,
            .fogColor = 0,
            .cloudColor = 0,
            .cloudHeight = 80,
            .surroundingGroundType = 0x07,
            .surroundingGroundHeight = 29,
            .surroundingWaterType = 0x08,
            .surroundingWaterHeight = 31
    };

    LevelMap map = {
            .length = 256,
            .height = 64,
            .width = 256,

            .spawnX = 128,
            .spawnZ = 128,
            .spawnY = 48,

            .blocks = NULL,
            .data = NULL
    };


    LevelEntityList e_list = {
            .count = 0,
            .entities = NULL
    };

    LevelTileEntityList te_list = {
            .count = 0,
            .entities = NULL
    };

    level.about = abt;
    level.environment =  env;
    level.map = map;
    level.entities = e_list;
    level.tileEntities = te_list;

    CC_Internal_Log_Message(CC_LOG_DEBUG, "Generated base level template");
}

void CrossCraft_World_Deinit() {
    CC_Internal_Log_Message(CC_LOG_INFO, "Destroying the world");

    if(level.map.blocks)
        free(level.map.blocks);

    if(level.map.data)
        free(level.map.data);

    if(level.entities.entities)
        free(level.entities.entities);

    if(level.tileEntities.entities)
        free(level.tileEntities.entities);

    CC_Internal_Log_Message(CC_LOG_DEBUG, "World freed");
}

void CrossCraft_World_Create_Map(uint8_t size) {
    switch(size) {
        case WORLD_SIZE_SMALL: {
            level.map.length = 128;
            level.map.width = 128;
            level.map.height = 64;
            break;
        }
        case WORLD_SIZE_HUGE: {
            level.map.length = 512;
            level.map.width = 512;
            level.map.height = 64;
            break;
        }
        case WORLD_SIZE_NORMAL:
        default: {
            level.map.length = 256;
            level.map.width = 256;
            level.map.height = 64;
            break;
        }
    }

    uint32_t blockCount = level.map.length * level.map.height * level.map.width;
    level.map.blocks = calloc(sizeof(uint8_t), blockCount);
    level.map.data = calloc(sizeof(uint8_t), blockCount);
}

/**
 * @brief Tries to load a world
 * @returns If the world was loaded
 */
bool CrossCraft_World_TryLoad(uint8_t slot) {
    if(slot >= 5) {
        return false;
    }
    //TODO: Load World NBT
    return false;
}

/**
 * @brief Generates the world
 * @TODO Offer a callback for world percentage
 */
void CrossCraft_World_GenerateMap() {
    CrossCraft_WorldGenerator_Generate(&level.map);
}

/**
 * @brief Spawn the player into the world
 */
void CrossCraft_World_Spawn() {

}

LevelMap* CrossCraft_World_GetMapPtr() {
    return &level.map;
}