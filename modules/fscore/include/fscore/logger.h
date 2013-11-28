#ifndef FS_LOGGER_H
#define FS_LOGGER_H

#include <string>

#include "fscore/types.h"

namespace fs
{
    namespace Logger
    {
        enum DisplayFlags
        {
            DisplayFlagWriteToFile = 1 << 0,
            DisplayFlagWriteToStdOut = 1 << 1
        };

        void init(const char* configFilename);
        void destroy();
        void log(const string& tag, const char* funcName, const char* sourceFile,
                 u32 lineNum, const string& format, ...);
        void setDisplayFlags(const string& tag, u32 flags);
    }
}

#define FS_LOG(tag, format, ...) \
    do \
    { \
        fs::Logger::log((tag), __FUNCTION__, __FILE__, __LINE__, (format), __VA_ARGS__) \
    } \
    while(0) \

#define FS_FATAL(format, ...) \
    do \
    { \
        FS_LOG("FATAL", format, __VAR_ARGS__); \
        FS_ASSERT_MSG_FORMATTED(!"LOG FATAL", (format), __VA_ARGS__); \
    } \
    while(0) \

#define FS_ERROR(format, ...) \
    do \
    { \
        FS_LOG("ERROR", format, __VAR_ARGS__); \
        FS_ASSERT_MSG_FORMATTED(!"LOG ERROR", (format), __VA_ARGS__); \
    } \
    while(0) \

#define FS_WARNING(format, ...) FS_LOG("WARNING", format, __VA_ARGS__)
#define FS_INFO(format, ...) FS_LOG("INFO", format, __VAR_ARGS__)
#define FS_DEBUG(format, ...) FS_LOG("DEBUG", format, __VAR_ARGS__)

#endif
