#include <CrossCraft/core.h>
#include <CrossCraft/log.h>
#include <CrossCraft/world.h>
#include <CrossCraft/entity.h>

// Initializes the CrossCraft Core, including the logging system and the entity and world managers.
void CrossCraft_Core_Init() {
    CC_Internal_Log_Init();
    CC_Internal_Log_Message(CC_LOG_INFO, "CrossCraft Core Initialized.");

    CrossCraft_EntityMan_Init();
    CrossCraft_World_Init();
}

// Deinitializes the CrossCraft Core, including the entity and world managers and the logging system.
void CrossCraft_Core_Deinit() {
    CrossCraft_World_Deinit();
    CrossCraft_EntityMan_Deinit();

    CC_Internal_Log_Message(CC_LOG_INFO, "CrossCraft Core Shutdown.");
    CC_Internal_Log_Deinit();
}

// Updates the state of the CrossCraft Core, including the entity manager.
void CrossCraft_Core_Tick() {
    CrossCraft_EntityMan_Tick();
}

static DifficultyLevel CC_DifficultyLevel = DIFFICULTY_NORMAL;

// Sets the difficulty level
void CrossCraft_SetDifficulty(DifficultyLevel level) {
    CC_DifficultyLevel = level;
}

// Gets the difficulty level
DifficultyLevel CrossCraft_GetDifficulty() {
    return CC_DifficultyLevel;
}