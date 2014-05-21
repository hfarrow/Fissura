#include "fscore/debugging/utils.h"

#include <execinfo.h>
#include <cxxabi.h>
#include <stdio.h>

#include "fscore/debugging/utils.h"
#include "fscore/utils/types.h"
#include "fscore/debugging/assert.h"

#include "fscore/utils/types.h"

using namespace fs;

namespace fs
{
namespace internal
{
    template<>
    size_t StackTraceUtil<PLATFORM_ID>::getStackTrace(void** frames, size_t maxFrames)
    {
        return backtrace(frames, maxFrames);
    }

    template<>
    char* StackTraceUtil<PLATFORM_ID>::demangleStackSymbol(char* functionSymbol)
    {
        int validCppName;
        char* functionName = abi::__cxa_demangle(functionSymbol, nullptr, 0, &validCppName);
        if(validCppName == 0)
        {
            return functionName;
        }
        else
        {
            return nullptr;
        }
    }

    template<>
    char* StackTraceUtil<PLATFORM_ID>::getCaller(void** frames, size_t numFrames, size_t framesToSkip)
    {
        const size_t bufferSize = 8192;
        static char buffer[bufferSize];

        char** strings = backtrace_symbols(frames, numFrames);

        char* current = buffer;
        for(u32 i = 1 + framesToSkip; i < numFrames; i++)
        {
            char* stackSymbol = *(strings + i);
            char functionSymbol[1024];
            char moduleName[1024];
            char address[1024];
            int level;
            int offset;
            sscanf(stackSymbol, "%d %s %s %s %*s %d", &level, moduleName, address, functionSymbol, &offset);

            char* demangled = demangleStackSymbol(functionSymbol);

            if(demangled)
            {
                char frameInfo[1024];
                snprintf(frameInfo, 1024, "[%d] %s\t%s %s + %d", level, moduleName, demangled, address, offset);
                strncpy(current, frameInfo, bufferSize - (current - buffer));
                current += strlen(frameInfo);
                free(demangled);
            }
            else
            {
                strncpy(current, stackSymbol, bufferSize - (current - buffer));
                current += strlen(stackSymbol);
            }


            if(current < buffer + bufferSize)
            {
                *current = '\n';
                current++;
            }
        }
        
        free(strings);

        return buffer;
    }
}
}
