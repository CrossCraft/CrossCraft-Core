#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "level.h"

#ifdef __cplusplus
extern "C" {
#endif

void CrossCraft_WorldGenerator_Init(int32_t seed);
void CrossCraft_WorldGenerator_Generate(LevelMap* map);

#ifdef __cplusplus
};
#endif