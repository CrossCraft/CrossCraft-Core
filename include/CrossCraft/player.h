#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"

void CrossCraft_Player_Init();
void CrossCraft_Player_Deinit();

MCVector3 CrossCraft_Player_GetPosition();
void CrossCraft_Player_SetPosition(MCVector3 pos);

PlayerInventory CrossCraft_Player_GetInventory();
void CrossCraft_Player_SetInventory(PlayerInventory* playerInventory);

#ifdef __cplusplus
}
#endif