#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "level.h"

void CrossCraft_World_Init();
void CrossCraft_World_Deinit();

/**
 * This method should ONLY be used by a client.
 * @return Returns a pointer to the level
 */
LevelMap* CrossCraft_World_GetMapPtr();

#ifdef __cplusplus
};
#endif