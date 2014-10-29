#ifndef FS_GLOBAL_FIXTURE_H
#define FS_GLOBAL_FIXTURE_H

#include <SDL.h>
#include "fscore.h"
#include "fslog.h"

using namespace fs;

class GlobalFixture
{
public:
    static GlobalFixture*& instance()
    {
        static GlobalFixture* s_inst = 0;
        return s_inst;
    }

    GlobalFixture() :
        basePath(SDL_GetBasePath()),
        logger(path("content/logger.xml"), "fissura")
    {
        instance() = this;
        std::remove(path("fissura.log"));
        // logger.setConsoleSurpressed(true);
        log::setRootLogger(&logger);
    }

    ~GlobalFixture()
    {
        SDL_free(basePath);
    }

    using PathBuffer = char[512];
    void path(const char* inPath, PathBuffer out)
    {
        auto fullPath = std::string(basePath) + std::string(inPath);
        FS_ASSERT(fullPath.length() < sizeof(PathBuffer));
        strncpy(out, fullPath.c_str(), sizeof(PathBuffer));
    }

    const char* path(const char* inPath)
    {
        path(inPath, lastPath);
        return lastPath;
    }

    char* basePath;
    PathBuffer lastPath;
    Logger logger;
};

#endif

