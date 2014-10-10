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
        logger(path("content/logger.xml").c_str(), "fissura")
    {
        instance() = this;
        std::remove(path("fissura.log").c_str());
        logger.setConsoleSurpressed(true);
        log::setRootLogger(&logger);
    }

    ~GlobalFixture()
    {
        SDL_free(basePath);
    }

    std::string path(const char* path)
    {
        return std::string(basePath) + std::string(path);
    }

    char* basePath;
    Logger logger;
};

#endif

