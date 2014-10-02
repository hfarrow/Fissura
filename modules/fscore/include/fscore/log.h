#ifndef FS_CORE_CORE_LOG_H
#define FS_CORE_CORE_LOG_H

#include "fscore/types.h"
#include "fscore/macros.h"

#include <iostream>
#define FS_PRINT(x) std::cout << x << std::endl

#define FS_CORE_LOGF(tag, format, ...)    \
    do \
    { \
        auto logger = fs::core::getLogger(); \
        if(logger) logger->log((tag), __FUNCTION__, __FILE__, __LINE__, (format), __VA_ARGS__); \
    } \
    while(0)

#define FS_CORE_LOG(tag, message) \
    FS_CORE_LOGF(tag, "%s", message)

#define FS_CORE_INFOF(format, ...)  FS_CORE_LOGF("INFO", format, __VA_ARGS__)
#define FS_CORE_INFO(message)   FS_CORE_LOG("INFO", message)

namespace fs
{
    class ILogger
    {
    public:
        virtual ~ILogger() {}
        virtual void log(const char* tag, const char* funcName, const char* sourceFile, u32 lineNum,
                         const char* format, ...) FS_ABSTRACT;
    };

    class ConsoleLogger : public ILogger
    {
    public:
        virtual void log(const char* tag, const char* funcName, const char* sourceFile, u32 lineNum,
                         const char* format, ...) override;
    };

    namespace core
    {
        void setLogger(ILogger* pLogger);
        ILogger* getLogger();
    }
}

#endif

