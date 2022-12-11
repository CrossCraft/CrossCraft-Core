#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "level.h"

#ifdef __cplusplus
extern "C" {
#endif

void CrossCraft_WorldGenerator_Init(int32_t seed);
void CrossCraft_WorldGenerator_Generate_Original(LevelMap* map);
void CrossCraft_WorldGenerator_Generate_Flat(LevelMap* map);
void CrossCraft_WorldGenerator_Generate_Woods(LevelMap* map);
void CrossCraft_WorldGenerator_Generate_Island(LevelMap* map);;
void CrossCraft_WorldGenerator_Generate_Floating(LevelMap* map);

#ifdef __cplusplus
};
#endif