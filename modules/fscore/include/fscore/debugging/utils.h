#ifndef FS_DEBUGGING_UTILS_H
#define FS_DEBUGGING_UTILS_H

#include "fscore/utils/types.h"
#include "fscore/utils/platforms.h"

namespace fs
{
    namespace internal
    {
        template<u32 PlatformID>
        class StackTraceUtil
        {
            public:
                static size_t getStackTrace(void** frames, size_t maxFrames);
                static char* getCaller(void** frames, size_t numFrames, size_t framesToSkip);
                static char* demangleStackSymbol(char* functionSymbol);
        };
    }

    using StackTraceUtil = internal::StackTraceUtil<PLATFORM_ID>;
}

#endif
