#ifndef FS_GLOBAL_FIXTURE_H
#define FS_GLOBAL_FIXTURE_H

#include <SDL.h>
#include "fscore.h"

class GlobalFixture
{
public:
    static GlobalFixture*& instance()
    {
        static GlobalFixture* s_inst = 0;
        return s_inst;
    }

    GlobalFixture()
    {
       instance() = this;
        basePath = SDL_GetBasePath();
        std::remove(path("fissura.log").c_str());
        // fs::Logger::setSurpressStdOutput(true);
        fs::Logger::init(path("content/logger.xml").c_str());
    }

    ~GlobalFixture()
    {
        fs::Logger::destroy();
        SDL_free(basePath);
    }

    std::string path(const char* path)
    {
        return std::string(basePath) + std::string(path);
    }

    char* basePath;
};

#endif
