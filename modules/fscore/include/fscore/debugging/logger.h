#ifndef FS_LOGGER_H
#define FS_LOGGER_H

#include <SDL.h>
#include <boost/format.hpp>

#include "fscore/utils/types.h"
#include "fscore/utils/string.h"
#include "fscore/utils/macros.h"

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


#define FS_LOGF(tag, format, ...) \
    do \
    { \
        fs::Logger::log((tag), FS_MAKE_FORMAT(format, __VA_ARGS__).str(), __FUNCTION__, __FILE__, __LINE__); \
    } \
    while(0)

#define FS_FATALF(format, ...) \
    do \
    { \
        auto message = FS_MAKE_FORMAT(format, __VA_ARGS__); \
        fs::Logger::log("FATAL", message.str(), __FUNCTION__, __FILE__, __LINE__); \
        SDL_assert(!"LOG FATAL" && message.str().c_str()); \
    } \
    while(0)

#define FS_ERRORF(format, ...) \
    do \
    { \
        auto message = FS_MAKE_FORMAT(format, __VA_ARGS__); \
        fs::Logger::log("ERROR", message.str(), __FUNCTION__, __FILE__, __LINE__); \
        SDL_assert(!"LOG ERROR" && message.str().c_str()); \
    } \
    while(0)

#define FS_WARNF(format, ...) FS_LOGF("WARN", format, __VA_ARGS__)
#define FS_INFOF(format, ...) FS_LOGF("INFO", format, __VA_ARGS__)
#define FS_DEBUGF(format, ...) FS_LOGF("DEBUG", format, __VA_ARGS__)

#define FS_LOG(tag, message) FS_LOGF(tag, "%1%", message)
#define FS_FATAL(message) FS_FATALF("%1%", message)
#define FS_ERROR(message) FS_ERRORF("%1%", message)
#define FS_WARN(message) FS_LOG("WARN", message)
#define FS_INFO(message) FS_LOG("INFO", message)
#define FS_DEBUG(message) FS_LOG("DEBUG", message)
#endif
