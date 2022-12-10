#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "level.h"

#define WORLD_SIZE_SMALL 0
#define WORLD_SIZE_NORMAL 1
#define WORLD_SIZE_HUGE 2

void CrossCraft_World_Init();
void CrossCraft_World_Deinit();

/**
 * Creates an empty map with a selected world size.
 * @param size Size Constant
 */
void CrossCraft_World_Create_Map(uint8_t size);

/**
 * This method should ONLY be used by a client in single-player or a server for internal use.
 * @return Returns a pointer to the level
 */
LevelMap* CrossCraft_World_GetMapPtr();

/**
 * @brief Spawn the player into the world
 */
void CrossCraft_World_Spawn();

/**
 * @brief Tries to load a world
 * @returns If the world was loaded
 */
bool CrossCraft_World_TryLoad(uint8_t slot, const char* prefix);

/**
 * @brief Generates the world
 * @TODO Offer a callback for world percentage
 */
void CrossCraft_World_GenerateMap();

void CrossCraft_World_AddLight(uint16_t x, uint16_t y, uint16_t z, uint16_t light, uint32_t* updateIDs);
void CrossCraft_World_RemoveLight(uint16_t x, uint16_t y, uint16_t z, uint16_t light, uint32_t* updateIDs);

#ifdef __cplusplus
};
#endif