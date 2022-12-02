#include <CrossCraft/core.h>
#include <CrossCraft/log.h>

void CrossCraft_Core_Init() {
    CC_Internal_Log_Init();
    CC_Internal_Log_Message(CC_LOG_INFO, "CrossCraft Core Initialized.");
}

void CrossCraft_Core_Deinit() {
    CC_Internal_Log_Message(CC_LOG_INFO, "CrossCraft Core Shutdown.");
    CC_Internal_Log_Deinit();
}