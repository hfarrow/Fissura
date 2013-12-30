#include "fscore/debugging/logger.h"

#include <stdlib.h>
#include <iostream>
#include <fstream>

#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <tinyxml.h>
#include <SDL.h>

#include "fscore/utils/types.h"
#include "fscore/memory/new.h"
#include "fscore/utils/utils.h"
#include "fscore/utils/globals.h"

using namespace fs;
using namespace boost::posix_time;
using namespace boost::gregorian;

// Constants
static const char* LOG_FILENAME = "fissura.log";
const u8 DEFAULT_FLAG_FATAL = Logger::DisplayFlagWriteToFile | Logger::DisplayFlagWriteToStdOut;
const u8 DEFAULT_FLAG_ERROR = Logger::DisplayFlagWriteToFile | Logger::DisplayFlagWriteToStdOut;
const u8 DEFAULT_FLAG_WARNING = Logger::DisplayFlagWriteToFile | Logger::DisplayFlagWriteToStdOut;
const u8 DEFAULT_FLAG_LOG = Logger::DisplayFlagWriteToFile | Logger::DisplayFlagWriteToStdOut;
const u8 DEFAULT_FLAG_DEBUG = 0;

// Singleton
class LogManager;
static LogManager* s_pLogManager = nullptr;

class LogManager
{
public:
    typedef Map<std::string, u8> Tags;
    Tags tags;

    LogManager();
    ~LogManager();
    void init(const char* configFilename);

    void log(const std::string& tag, const std::string& message, const char* funcName,
             const char* sourceFile, u32 lineNum);
    void setDisplayFlags(const std::string& tag, u32 flags);

private:
    boost::mutex _tagMutex;

    void outputFinalBufferToLogs(const std::string& finalBuffer, u32 flags);
    void writeToLogFile(const std::string& data);
    void getOutputBuffer(std::string& outOutputBuffer, const std::string& tag, const std::string& message,
                         const char* funcName, const char* sourceFile, u32 lineNum);
};

LogManager::LogManager()
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
    FS_INFOF(boost::format("LogManager::init('%1%')") % configFilename);
    if(configFilename)
    {
        TiXmlDocument configFile(configFilename);
        if(configFile.LoadFile())
        {
            FS_INFOF(boost::format("LogManager::init config xml loaded -> %1%") % configFile);
            TiXmlElement* pRoot = configFile.RootElement();
            if(!pRoot)
                return;

            for(TiXmlElement* pNode = pRoot->FirstChildElement(); pNode; pNode = pNode->NextSiblingElement())
            {
                u32 flags = 0;
                std::string tag(pNode->Attribute("tag"));

                i32 debugger = 0;
                pNode->Attribute("debugger", &debugger);
                if(debugger)
                    flags |= Logger::DisplayFlagWriteToStdOut;

                i32 logfile = 0;
                pNode->Attribute("file", &logfile);
                if(logfile)
                    flags |= Logger::DisplayFlagWriteToFile;

                setDisplayFlags(tag, flags);
            }
        }
        else
        {
            FS_WARNF(boost::format("LogManager::init failed to load config xml: %1%") % configFile.ErrorDesc());
        }
    }
}

void LogManager::log(const std::string& tag, const std::string& message, const char* funcName,
                const char* sourceFile, u32 lineNum)
{
    _tagMutex.lock();
    auto findIt = tags.find(tag);
    if(findIt != tags.end())
    {
        _tagMutex.unlock();
        
        std::string buffer;
        getOutputBuffer(buffer, tag, message, funcName, sourceFile, lineNum);
        outputFinalBufferToLogs(buffer, findIt->second);
    }
    else
    {
        _tagMutex.unlock();
    }
}

void LogManager::setDisplayFlags(const std::string& tag, u32 flags)
{
    boost::lock_guard<boost::mutex> lock(_tagMutex);
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

void LogManager::outputFinalBufferToLogs(const std::string& finalBuffer, u32 flags)
{
    if((flags & Logger::DisplayFlagWriteToFile) > 0)
        writeToLogFile(finalBuffer);
    if((flags & Logger::DisplayFlagWriteToStdOut) > 0)
        Logger::printStringFormatted(boost::format(finalBuffer));
}

void LogManager::writeToLogFile(const std::string& data)
{
    std::ofstream output(LOG_FILENAME, std::ios::out | std::ios::app);
    if(output.good())
    {
        output << data;
    }
}

void LogManager::getOutputBuffer(std::string& outOutputBuffer, const std::string& tag, const std::string& message,
                            const char* funcName, const char* sourceFile, u32 lineNum)
{
    if(!tag.empty())
        outOutputBuffer = "[" + tag + "] " + message;
    else
        outOutputBuffer = "[NO-TAG] " + message;

    outOutputBuffer += "\n\t at ";

    if(funcName != nullptr)
    {
        outOutputBuffer += funcName;
    }
    if(lineNum != 0)
    {
        outOutputBuffer += ":";
        outOutputBuffer += std::to_string(lineNum);
    }
    if(sourceFile != nullptr)
    {
        outOutputBuffer += " in ";
        outOutputBuffer += sourceFile;
    }
    
    outOutputBuffer += " (";
    ptime pt = microsec_clock::local_time();
    time_duration td = pt.time_of_day();
    auto hours = td.hours();
    auto minutes = td.minutes();
    auto seconds = td.seconds();
    auto milliseconds = td.total_milliseconds() - ((hours * 3600 + minutes * 60 + seconds) * 1000);
    outOutputBuffer += (boost::format("%1%:%2%:%3%:%4%") % hours % minutes % seconds % milliseconds).str();
    outOutputBuffer += ")\n";
}

namespace fs
{
namespace Logger
{
    void init(const char* configFilename)
    {
        if(!s_pLogManager)
        {
            s_pLogManager = FS_NEW(LogManager)();
        }

        s_pLogManager->init(configFilename);
    }

    void destroy()
    {
        FS_DELETE(s_pLogManager);
        s_pLogManager = nullptr;
    }

    void log(const std::string& tag, const std::string& message, const char* funcName, const char* sourceFile,
                     u32 lineNum)
    {
        if(s_pLogManager)
        {
            s_pLogManager->log(tag, message, funcName, sourceFile, lineNum);
        }
    }

    void setDisplayFlags(const std::string& tag, u32 flags)
    {
        FS_ASSERT(s_pLogManager);
        s_pLogManager->setDisplayFlags(tag, flags);
    }

    void printStringFormatted(boost::format format)
    {
        if(!surpressStdOutput)
        {
#ifdef WIN32
            OutputDebugStringA(format.str().c_str());
#endif
            std::cout << format;
        }
    }
    bool surpressStdOutput = false;
}
}
