#include "fscore/log.h"

#include <cstdio>
#include <iostream>
#include <stdarg.h>

using namespace fs;

static ConsoleLogger defaultLogger;
static ILogger* _pLogger = &defaultLogger;

void ConsoleLogger::log(const char* tag, const char* funcName, const char* sourceFile, u32 lineNum,
                         const char* format, ...)
{
    va_list args;
    va_start(args, format);

    char messageBuffer[1792] = {0};
    std::snprintf(messageBuffer, sizeof(messageBuffer), format, args);

    va_end(args);

    char outputBuffer[2048] = {0};
    std::snprintf(outputBuffer, sizeof(outputBuffer), "[%s] %s \n    %s in %s:%u",
            tag, messageBuffer, funcName, sourceFile, lineNum);

    std::cout << outputBuffer << std::endl;

#ifdef WIN32
    OutputDebugStringA(outputBuffer);
#endif
}

ILogger* core::getLogger()
{
    return _pLogger;
}

void core::setLogger(ILogger* pLogger)
{
    _pLogger = pLogger;
}
