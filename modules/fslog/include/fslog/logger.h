#ifndef FS_LOGGER_H
#define FS_LOGGER_H

#include <chrono>
#include <ctime>

#include <SDL.h>
#include <boost/format.hpp>

#include "fscore/types.h"
#include "fscore/macros.h"
#include "fscore/log.h"

#include "fsmem/allocators/stl_allocator.h"
#include "fsmem/stl_types.h"

namespace fs
{
    class Logger : public ILogger
    {
    public:
        using Tags = DebugMap<DebugString, u8>;
        using TagAllocator = DebugMapAllocator<DebugString, u8>;

        enum DisplayFlags
        {
            DisplayFlagWriteToFile =    1 << 0,
            DisplayFlagWriteToStdOut =  1 << 1,
            DisplayFlagDetailed =       1 << 2
        };

        Logger(const char* configFilename, const char* name="fissura.log");
        virtual ~Logger();

        // ILogger implementation
        virtual void log(const char* tag, const char* funcName, const char* sourceFile, u32 lineNum,
                         const char* format, ...) override;

        // A more conveneint log method that uses fsmem
        void log(const DebugString& tag, const DebugString& message, const char* funcName,
                const char* sourceFile, u32 lineNum);

        void setDisplayFlags(const DebugString& tag, u32 flags);
        bool isConsoleSurpressed() const;
        void setConsoleSurpressed(bool surpressed);
        void addChild(Logger* pLogger);

    private:
        using clock = std::chrono::high_resolution_clock;
        using time_point = std::chrono::high_resolution_clock::time_point;

        void logWithoutLock(const DebugString& tag, const DebugString& message, const char* funcName,
                const char* sourceFile, u32 lineNum);

        void outputFinalBufferToLogs(const DebugString& finalBuffer, u32 flags);
        void flushOutputBuffer();
        void writeToLogFile(const DebugString& data);
        void getOutputBuffer(DebugString& outOutputBuffer, const DebugString& tag, u32 flags, const DebugString& message,
                            const char* funcName, const char* sourceFile, u32 lineNum);

        void setParent(Logger* pLogger);

        const char* _configFileName;
        const char* _name;
        Logger* _pParent;
        bool _consoleSurpressed;
        Tags _tags;
        std::mutex _mutex;
        clock::time_point _startTime;
        DebugString _outputBuffer;
        clock::time_point _lastFlushTime;
    };

    namespace log
    {
        void setRootLogger(Logger* pLogger);
        Logger* getRootLogger();
    }
}


#define FS_LOGF(tag, format, ...) \
    do \
    { \
        fs::log::getRootLogger()->log((tag), FS_MAKE_FORMAT(format, __VA_ARGS__).str(), __FUNCTION__, __FILE__, __LINE__); \
    } \
    while(0)

#define FS_FATALF(format, ...) \
    do \
    { \
        auto message = FS_MAKE_FORMAT(format, __VA_ARGS__); \
        fs::log::getRootLogger()->log("FATAL", message.str(), __FUNCTION__, __FILE__, __LINE__); \
        SDL_assert(!"LOG FATAL" && message.str().c_str()); \
    } \
    while(0)

#define FS_ERRORF(format, ...) \
    do \
    { \
        auto message = FS_MAKE_FORMAT(format, __VA_ARGS__); \
        fs::log::getRootLogger()->log("ERROR", message.str(), __FUNCTION__, __FILE__, __LINE__); \
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
