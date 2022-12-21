#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "world.h"

void CrossCraft_Core_Init();
void CrossCraft_Core_Deinit();

void CrossCraft_Core_Tick();

uint32_t CrossCraft_Core_GetTickCount();

typedef enum {
    DIFFICULTY_PEACEFUL,
    DIFFICULTY_EASY,
    DIFFICULTY_NORMAL,
    DIFFICULTY_HARD,
} DifficultyLevel;

void CrossCraft_SetDifficulty(DifficultyLevel level);

DifficultyLevel CrossCraft_GetDifficulty();

#ifdef __cplusplus
};
#endif