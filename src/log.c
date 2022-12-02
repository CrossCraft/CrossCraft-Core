#include <CrossCraft/log.h>
#include <stdio.h>

static FILE* CC_Internal_LogFile = NULL;
static int CC_Internal_Log_Level = CC_LOG_INFO;

void CC_Internal_Log_Init() {
    CC_Internal_LogFile = fopen("crosscraft.log", "w+");
    CC_Internal_Log_SetDepth(CC_LOG_TRACE);
}
void CC_Internal_Log_Deinit() {
    if(CC_Internal_LogFile != NULL)
        fclose(CC_Internal_LogFile);
}

void CC_Internal_Log_SetDepth(int level) {
    CC_Internal_Log_Level = level;
}

const char* CC_Internal_Log_GetLevelStr(int level) {
    switch(level) {
        case CC_LOG_TRACE:
            return "[Trace]:";

        case CC_LOG_DEBUG:
            return "[Debug]:";

        case CC_LOG_INFO:
            return "[Info]:";

        case CC_LOG_WARN:
            return "[Warning]:";

        case CC_LOG_ERROR:
        default:
            return "[Error]:";
    }
}

void CC_Internal_Log_Message(int level, const char* msg) {
    if(level == CC_LOG_NONE)
        return;

    if(level > CC_Internal_Log_Level)
        return;

    fprintf(stdout, "%s %s\n", CC_Internal_Log_GetLevelStr(level), msg);
    fprintf(CC_Internal_LogFile, "%s %s\n", CC_Internal_Log_GetLevelStr(level), msg);
}