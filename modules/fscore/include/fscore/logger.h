#ifndef FS_LOGGER_H
#define FS_LOGGER_H

#include <SDL.h>
#include <boost/format.hpp>

#include "fscore/types.h"
#include "fscore/string.h"

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
        void log(const std::string& tag, const std::string& message, 
                 const char* funcName, const char* sourceFile, u32 lineNum);
        void setDisplayFlags(const std::string& tag, u32 flags);
        void printStringFormatted(boost::format);
        extern bool surpressStdOutput;
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

#define FS_LOG(tag, message) FS_LOGF(tag, boost::format((message)))
#define FS_FATAL(message) FS_FATALF(boost::format((message)))
#define FS_ERROR(message) FS_ERRORF(boost::format((message)))
#define FS_WARN(message) FS_LOG("WARN", message)
#define FS_INFO(message) FS_LOG("INFO", message)
#define FS_DEBUG(message) FS_LOG("DEBUG", message)
#define FS_WARNF(format) FS_LOGF("WARN", format)
#define FS_INFOF(format) FS_LOGF("INFO", format)
#define FS_DEBUGF(format) FS_LOGF("DEBUG", format)

#endif