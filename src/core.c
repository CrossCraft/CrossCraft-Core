#include <CrossCraft/core.h>
#include <CrossCraft/log.h>
#include <CrossCraft/world.h>
#include <CrossCraft/entity.h>

void CrossCraft_Core_Init() {
    CC_Internal_Log_Init();
    CC_Internal_Log_Message(CC_LOG_INFO, "CrossCraft Core Initialized.");

    CrossCraft_EntityMan_Init();
    CrossCraft_World_Init();
}

void CrossCraft_Core_Deinit() {
    CrossCraft_World_Deinit();
    CrossCraft_EntityMan_Deinit();

    CC_Internal_Log_Message(CC_LOG_INFO, "CrossCraft Core Shutdown.");
    CC_Internal_Log_Deinit();
}

void CrossCraft_Core_Tick() {
    CrossCraft_EntityMan_Tick();
}