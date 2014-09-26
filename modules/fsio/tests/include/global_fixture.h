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
};

#endif
