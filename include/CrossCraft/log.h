#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define CC_LOG_NONE 0
#define CC_LOG_ERROR 1
#define CC_LOG_WARN 2
#define CC_LOG_INFO 3
#define CC_LOG_DEBUG 4
#define CC_LOG_TRACE 5

void CC_Internal_Log_Init();
void CC_Internal_Log_Deinit();

void CC_Internal_Log_SetDepth(int level);
void CC_Internal_Log_Message(int level, const char* msg);

#ifdef __cplusplus
};
#endif