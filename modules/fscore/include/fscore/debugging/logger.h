#ifndef FS_LOGGER_H
#define FS_LOGGER_H

#include <SDL.h>
#include <boost/format.hpp>

#include "fscore/utils/types.h"
#include "fscore/utils/string.h"
#include "fscore/memory/memory_arena.h"
#include "fscore/memory/allocation_policy.h"
#include "fscore/memory/bounds_checking_policy.h"
#include "fscore/memory/memory_tagging_policy.h"
#include "fscore/memory/memory_tracking_policy.h"
#include "fscore/memory/thread_policy.h"
#include "fscore/memory/heap_allocator.h"

namespace fs
{
    namespace Logger
    {
        enum DisplayFlags
        {
            DisplayFlagWriteToFile =    1 << 0,
            DisplayFlagWriteToStdOut =  1 << 1,
            DisplayFlagDetailed =       1 << 2
        };

        void init(const char* configFilename);
        void destroy();
        void log(const DebugString& tag, const DebugString& message, 
                 const char* funcName, const char* sourceFile, u32 lineNum);
        void setDisplayFlags(const DebugString& tag, u32 flags);
        void printStringFormatted(fs::dformat format);
        bool getSurpressStdOutput();
        void setSurpressStdOutput(bool surpress);
    }
}

#define FS_LOGF(tag, format) \
    do \
    { \
        fs::Logger::log((tag), (format).str(), __FUNCTION__, __FILE__, __LINE__); \
    } \
    while(0)

#define FS_FATALF(format) \
    do \
    { \
        FS_LOG("FATAL", format); \
        SDL_assert(!"LOG FATAL" && (format).str().c_str()); \
    } \
    while(0)

#define FS_ERRORF(format) \
    do \
    { \
        FS_LOG("ERROR", format); \
        SDL_assert(!"LOG ERROR" && (format).str().c_str()); \
    } \
    while(0)

#define FS_LOG(tag, message) FS_LOGF(tag, fs::dformat((message)))
#define FS_FATAL(message) FS_FATALF(fs::dformat((message)))
#define FS_ERROR(message) FS_ERRORF(fs::dformat((message)))
#define FS_WARN(message) FS_LOG("WARN", message)
#define FS_INFO(message) FS_LOG("INFO", message)
#define FS_DEBUG(message) FS_LOG("DEBUG", message)
#define FS_WARNF(format) FS_LOGF("WARN", format)
#define FS_INFOF(format) FS_LOGF("INFO", format)
#define FS_DEBUGF(format) FS_LOGF("DEBUG", format)

#endif
