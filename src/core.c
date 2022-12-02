#include <CrossCraft/core.h>
#include <CrossCraft/log.h>
#include <CrossCraft/world.h>

void CrossCraft_Core_Init() {
    CC_Internal_Log_Init();
    CC_Internal_Log_Message(CC_LOG_INFO, "CrossCraft Core Initialized.");

    CrossCraft_World_Init();
}

void CrossCraft_Core_Deinit() {
    CrossCraft_World_Deinit();

    CC_Internal_Log_Message(CC_LOG_INFO, "CrossCraft Core Shutdown.");
    CC_Internal_Log_Deinit();
}