#include "fslog/logger.h"

#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <signal.h>
#include <fstream>
#include <thread>
#include <chrono>
#include <ctime>
#include <stdarg.h>

#include <tinyxml.h>
#include <SDL.h>

#include "fscore/types.h"
#include "fsmem/new.h"
#include "fsmem/debug/memory.h"
#include "fsmem/allocators/stl_allocator.h"

using namespace fs;

namespace fs
{
    namespace log
    {
        void abortHandler(i32 signum);
    }
}


// Constants
const u8 DEFAULT_FLAG_FATAL = Logger::DisplayFlagWriteToFile | Logger::DisplayFlagWriteToStdOut | Logger::DisplayFlagDetailed;
const u8 DEFAULT_FLAG_ERROR = Logger::DisplayFlagWriteToFile | Logger::DisplayFlagWriteToStdOut | Logger::DisplayFlagDetailed;
const u8 DEFAULT_FLAG_WARNING = Logger::DisplayFlagWriteToFile | Logger::DisplayFlagWriteToStdOut | Logger::DisplayFlagDetailed;
const u8 DEFAULT_FLAG_LOG = Logger::DisplayFlagWriteToFile;
const u8 DEFAULT_FLAG_DEBUG = 0;

#define FS_LOG_INTERNAL_INFO(format, ...)   \
    logWithoutLock("INFO", FS_MAKE_FORMAT(format, __VA_ARGS__).str(), __FUNCTION__, __FILE__, __LINE__)

Logger::Logger(const char* configFilename, const char* name) :
    _configFileName(configFilename),
    _name(name),
    _pParent(nullptr),
    _consoleSurpressed(false),
    _tags(TagAllocator()),
    _startTime(clock::now()),
    _lastFlushTime(clock::now())
{
    // set up the default log tags
    setDisplayFlags("FATAL", DEFAULT_FLAG_FATAL);
    setDisplayFlags("ERROR", DEFAULT_FLAG_ERROR);
    setDisplayFlags("WARN", DEFAULT_FLAG_WARNING);
    setDisplayFlags("INFO", DEFAULT_FLAG_LOG);
    setDisplayFlags("DEBUG", DEFAULT_FLAG_DEBUG);

    FS_LOG_INTERNAL_INFO("Logger::init log named '%1%'", _name);
    if(configFilename)
    {
        FS_LOG_INTERNAL_INFO("Logger::init from config: %1%", configFilename);
        TiXmlDocument configFile(_configFileName);
        if(configFile.LoadFile())
        {
            FS_LOG_INTERNAL_INFO("Logger::init config xml loaded: %1%", configFile);
            TiXmlElement* pRoot = configFile.RootElement();
            if(!pRoot)
                return;

            for(TiXmlElement* pNode = pRoot->FirstChildElement(); pNode; pNode = pNode->NextSiblingElement())
            {
                u32 flags = 0;
                DebugString tag(pNode->Attribute("tag"));

                i32 debugger = 0;
                pNode->Attribute("debugger", &debugger);
                if(debugger)
                    flags |= Logger::DisplayFlagWriteToStdOut;

                i32 logfile = 0;
                pNode->Attribute("file", &logfile);
                if(logfile)
                    flags |= Logger::DisplayFlagWriteToFile;

                i32 detailed = 0;
                pNode->Attribute("detailed", &detailed);
                if(detailed)
                {
                    flags |= Logger::DisplayFlagDetailed;
                }

                setDisplayFlags(tag, flags);

            }
        }
        else
        {
            FS_LOG_INTERNAL_INFO("Logger::init failed to load config xml: %1%", configFile.ErrorDesc());
        }
    }

    signal(SIGABRT, log::abortHandler);
    signal(SIGSEGV, log::abortHandler);
    signal(SIGILL, log::abortHandler);
    signal(SIGFPE, log::abortHandler);
}

Logger::~Logger()
{
    FS_LOG_INTERNAL_INFO("Logger destroyed: %1%", _name);
    flushOutputBuffer();
}

void Logger::log(const char* tag, const char* funcName, const char* sourceFile, u32 lineNum,
                         const char* format, ...)
{
    va_list args;
    va_start(args, format);

    char messageBuffer[1792] = {0};
    std::snprintf(messageBuffer, sizeof(messageBuffer), format, args);

    va_end(args);

    log(tag, messageBuffer, funcName, sourceFile, lineNum);
}

void Logger::log(const DebugString& tag, const DebugString& message, const char* funcName,
                const char* sourceFile, u32 lineNum)
{
    std::lock_guard<std::mutex> lock(_mutex);
    logWithoutLock(tag, message, funcName, sourceFile, lineNum);
}

void Logger::logWithoutLock(const DebugString& tag, const DebugString& message, const char* funcName,
                const char* sourceFile, u32 lineNum)
{
    auto findIt = _tags.find(tag);
    if(findIt != _tags.end())
    {
        DebugString buffer;
        getOutputBuffer(buffer, tag, findIt->second, message, funcName, sourceFile, lineNum);
        outputFinalBufferToLogs(buffer, findIt->second);
    }

    if(_pParent)
        _pParent->log(tag, message, funcName, sourceFile, lineNum);
}

void Logger::setDisplayFlags(const DebugString& tag, u32 flags)
{
    std::lock_guard<std::mutex> lock(_mutex);
    if(flags != 0)
    {
        auto findIt = _tags.find(tag);
        if(findIt == _tags.end())
        {
            _tags.insert(std::make_pair(tag, flags));
        }
        else
        {
            findIt->second = flags;
        }
    }
    else
    {
        _tags.erase(tag);
    }
}

bool Logger::isConsoleSurpressed() const
{
    return (_pParent && _pParent->isConsoleSurpressed()) || _consoleSurpressed;
}

void Logger::setConsoleSurpressed(bool surpressed)
{
    _consoleSurpressed = surpressed;
}

void Logger::addChild(Logger* pLogger)
{
    FS_ASSERT_MSG(!pLogger->_pParent, "Logger already has parent");
    pLogger->setParent(this);
}

void Logger::setParent(Logger* pLogger)
{
    _pParent = pLogger;
}

void Logger::outputFinalBufferToLogs(const DebugString& finalBuffer, u32 flags)
{
    if((flags & Logger::DisplayFlagWriteToFile) > 0)
    {
        _outputBuffer += finalBuffer;
        clock::time_point now = clock::now();
        clock::duration delta = now - _lastFlushTime;
        auto seconds = std::chrono::duration_cast<std::chrono::seconds>(delta);
        // Wait at least 30 seconds before flushing to log file.
        if(seconds.count() >  30)
        {
            flushOutputBuffer();
        }
    }

    if((flags & Logger::DisplayFlagWriteToStdOut && !isConsoleSurpressed()) > 0)
    {
#ifdef WIN32
        OutputDebugStringA(finalBuffer.c_str());
#endif
        std::cout << finalBuffer;
    }
}

void Logger::flushOutputBuffer()
{
    if(_outputBuffer.length() > 0)
    {
        writeToLogFile(_outputBuffer);
        _outputBuffer.clear();
        _lastFlushTime = clock::now();
    }
}

void Logger::writeToLogFile(const DebugString& data)
{
    DebugString name(_name);
    name += ".log";
    std::ofstream output(name.c_str(), std::ios::out | std::ios::app);
    if(output.good())
    {
        output << data;
    }
}

void Logger::getOutputBuffer(DebugString& outOutputBuffer, const DebugString& tag, u32 flags, const DebugString& message,
                            const char* funcName, const char* sourceFile, u32 lineNum)
{
    if(!tag.empty())
        outOutputBuffer = "[" + tag + "][" + DebugString(_name) + "] " + message;
    else
        outOutputBuffer = "[NO-TAG][" + DebugString(_name) + "] " + message;

    if((flags & Logger::DisplayFlagDetailed) > 0)
    {
        outOutputBuffer += "\n\t at ";

        if(funcName != nullptr)
        {
            outOutputBuffer += funcName;
        }
        if(sourceFile != nullptr)
        {
            outOutputBuffer += " in ";
            outOutputBuffer += sourceFile;
        }
        if(lineNum != 0)
        {
            outOutputBuffer += ":";
            char lineBuffer[6];
            snprintf(lineBuffer, sizeof(lineBuffer), "%d",lineNum);
            outOutputBuffer += lineBuffer;
        }

        clock::time_point now = clock::now();
        clock::duration delta = now - _startTime;

        {
            using namespace std::chrono;
            hours h = duration_cast<hours>(delta);
            delta -= h;
            minutes m = duration_cast<minutes>(delta);
            delta -= m;
            seconds s = duration_cast<seconds>(delta);
            delta -= s;
            microseconds ms = duration_cast<microseconds>(delta);
            delta -= ms;

            outOutputBuffer += (dformat(" (%1%h:%2%m:%3%.%4%s)") % h.count() % m.count() % s.count() % ms.count()).str();
        }
    }

    outOutputBuffer += "\n";
}

namespace fs
{
    namespace log
    {
        static Logger* gpRootLogger = nullptr;
        void setRootLogger(Logger* pLogger)
        {
            gpRootLogger = pLogger;
        }

        Logger* getRootLogger()
        {
            if(!gpRootLogger)
            {
                static Logger defaultLogger(nullptr, "default");
                return &defaultLogger;
            }

            return gpRootLogger;
        }

        void abortHandler(i32 signum)
        {
            // associate each signal with a signal name string.
            const char* name = NULL;
            switch( signum )
            {
                case SIGABRT: name = "SIGABRT";  break;
                case SIGSEGV: name = "SIGSEGV";  break;
                case SIGILL:  name = "SIGILL";   break;
                case SIGFPE:  name = "SIGFPE";   break;
            }

            if(gpRootLogger)
            {
                if ( name )
                    gpRootLogger->log("FATAL", (dformat("Caught signal %1% (%2%)") % signum % name).str(), __FUNCTION__, __FILE__, __LINE__);
                else
                    gpRootLogger->log("FATAL", (dformat("Caught signal %1%") % signum).str(), __FUNCTION__, __FILE__, __LINE__);
            }

            exit( signum );
        }
    }
}
