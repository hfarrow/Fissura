#include "fscore/debugging/logger.h"

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <ctime>

#include <tinyxml.h>
#include <SDL.h>

#include "fscore/utils/types.h"
#include "fscore/memory/new.h"
#include "fscore/debugging/memory.h"

using namespace fs;

// Constants
static const char* LOG_FILENAME = "fissura.log";
const u8 DEFAULT_FLAG_FATAL = Logger::DisplayFlagWriteToFile | Logger::DisplayFlagWriteToStdOut | Logger::DisplayFlagDetailed;
const u8 DEFAULT_FLAG_ERROR = Logger::DisplayFlagWriteToFile | Logger::DisplayFlagWriteToStdOut | Logger::DisplayFlagDetailed;
const u8 DEFAULT_FLAG_WARNING = Logger::DisplayFlagWriteToFile | Logger::DisplayFlagWriteToStdOut | Logger::DisplayFlagDetailed;
const u8 DEFAULT_FLAG_LOG = Logger::DisplayFlagWriteToFile | Logger::DisplayFlagWriteToStdOut;
const u8 DEFAULT_FLAG_DEBUG = 0;

// Singleton
class LogManager;
static LogManager* s_pLogManager = nullptr;

class LogManager
{
public:
    using Tags = Map<DebugString, u8, DebugArena>;
    using TagAllocator = StlAllocator<std::pair<DebugString, u8>, DebugArena>;
    Tags tags;

    LogManager();
    ~LogManager();
    void init(const char* configFilename);

    void log(const DebugString& tag, const DebugString& message, const char* funcName,
             const char* sourceFile, u32 lineNum);
    void setDisplayFlags(const DebugString& tag, u32 flags);

private:
    using clock = std::chrono::high_resolution_clock;
    using time_point = std::chrono::high_resolution_clock::time_point;

    std::mutex _tagMutex;
    clock::time_point _startTime;

    void outputFinalBufferToLogs(const DebugString& finalBuffer, u32 flags);
    void writeToLogFile(const DebugString& data);
    void getOutputBuffer(DebugString& outOutputBuffer, const DebugString& tag, u32 flags, const DebugString& message,
                         const char* funcName, const char* sourceFile, u32 lineNum);
};

LogManager::LogManager() :
    tags(TagAllocator(*memory::getDebugArena())),
    _startTime(clock::now())
{
    // set up the default log tags
    setDisplayFlags("FATAL", DEFAULT_FLAG_FATAL);
    setDisplayFlags("ERROR", DEFAULT_FLAG_ERROR);
    setDisplayFlags("WARN", DEFAULT_FLAG_WARNING);
    setDisplayFlags("INFO", DEFAULT_FLAG_LOG);
    setDisplayFlags("DEBUG", DEFAULT_FLAG_DEBUG);
}

LogManager::~LogManager()
{

}

void LogManager::init(const char* configFilename)
{
    FS_INFOF(dformat("LogManager::init('%1%')") % configFilename);
    if(configFilename)
    {
        TiXmlDocument configFile(configFilename);
        if(configFile.LoadFile())
        {
            FS_INFOF(dformat("LogManager::init config xml loaded: %1%") % configFile);
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
            FS_WARNF(dformat("LogManager::init failed to load config xml: %1%") % configFile.ErrorDesc());
        }
    }
}

void LogManager::log(const DebugString& tag, const DebugString& message, const char* funcName,
                const char* sourceFile, u32 lineNum)
{
    std::lock_guard<std::mutex> lock(_tagMutex);
    auto findIt = tags.find(tag);
    if(findIt != tags.end())
    {
        DebugString buffer;
        getOutputBuffer(buffer, tag, findIt->second, message, funcName, sourceFile, lineNum);
        outputFinalBufferToLogs(buffer, findIt->second);
    }
}

void LogManager::setDisplayFlags(const DebugString& tag, u32 flags)
{
    std::lock_guard<std::mutex> lock(_tagMutex);
    if(flags != 0)
    {
        auto findIt = tags.find(tag);
        if(findIt == tags.end())
        {
            tags.insert(std::make_pair(tag, flags));
        }
        else
        {
            findIt->second = flags;
        }
    }
    else
    {
        tags.erase(tag);
    }
}

void LogManager::outputFinalBufferToLogs(const DebugString& finalBuffer, u32 flags)
{
    if((flags & Logger::DisplayFlagWriteToFile) > 0)
        writeToLogFile(finalBuffer);
    if((flags & Logger::DisplayFlagWriteToStdOut) > 0)
        Logger::printStringFormatted(dformat(finalBuffer.c_str()));
}

void LogManager::writeToLogFile(const DebugString& data)
{
    std::ofstream output(LOG_FILENAME, std::ios::out | std::ios::app);
    if(output.good())
    {
        output << data;
    }
}

void LogManager::getOutputBuffer(DebugString& outOutputBuffer, const DebugString& tag, u32 flags, const DebugString& message,
                            const char* funcName, const char* sourceFile, u32 lineNum)
{
    if(!tag.empty())
        outOutputBuffer = "[" + tag + "] " + message;
    else
        outOutputBuffer = "[NO-TAG] " + message;

    if((flags & Logger::DisplayFlagDetailed) > 0)
    {
        outOutputBuffer += "\n\t at ";

        if(funcName != nullptr)
        {
            outOutputBuffer += funcName;
        }
        if(lineNum != 0)
        {
            outOutputBuffer += ":";
            outOutputBuffer += lineNum;
        }
        if(sourceFile != nullptr)
        {
            outOutputBuffer += " in ";
            outOutputBuffer += sourceFile;
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
namespace Logger
{
    static bool surpressStdOutput = false;

    void init(const char* configFilename)
    {
        if(!s_pLogManager)
        {
            s_pLogManager = FS_NEW(LogManager, *memory::getDebugArena())();
        }

        s_pLogManager->init(configFilename);
    }

    void destroy()
    {
        FS_DELETE(s_pLogManager, *memory::getDebugArena());
        s_pLogManager = nullptr;
    }

    void log(const DebugString& tag, const DebugString& message, const char* funcName, const char* sourceFile,
                     u32 lineNum)
    {
        if(s_pLogManager)
        {
            s_pLogManager->log(tag, message, funcName, sourceFile, lineNum);
        }
        else
        {
            FS_PRINT("[" << tag << "] " << message);
        }
    }

    void setDisplayFlags(const DebugString& tag, u32 flags)
    {
        FS_ASSERT(s_pLogManager);
        s_pLogManager->setDisplayFlags(tag, flags);
    }

    void printStringFormatted(dformat format)
    {
        if(!surpressStdOutput)
        {
#ifdef WIN32
            OutputDebugStringA(format.str().c_str());
#endif
            std::cout << format;
        }
    }

    bool getSurpressStdOutput()
    {
        return surpressStdOutput;
    }

    void setSurpressStdOutput(bool surpress)
    {
        surpressStdOutput = surpress;
    }
}
}
