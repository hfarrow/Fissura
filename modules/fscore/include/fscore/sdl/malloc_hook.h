#ifndef FS_SDL_MALLOC_HOOK_H
#define FS_SDL_MALLOC_HOOK_H

#include <cstdlib>

#include "fscore/utils/types.h"
#include "fscore/debugging/logger.h"

namespace fs
{
    // template<typename Arena>
    // class SdlHooks
    // {
    // public:
    //     void* allocate(size_t size, size_t alignment)
    //     {
    //         
    //     }
    // private:
    //     Arena _arena;
    // };
    
    class ISdlArena
    {
    public:
        virtual ~ISdlArena() {}

        virtual void* allocate(size_t size, size_t alignment, const SourceInfo& sourceInfo) = 0;
        virtual void free(void* ptr) = 0;
    };

    static fs::ISdlArena* pSdlArena = nullptr;
    void setSdlArena(fs::ISdlArena* pArena)
    {
        pSdlArena = pArena;
    }
}

void *SDL_malloc(size_t size)
{
    if(fs::pSdlArena)
        return fs::pSdlArena->allocate(size, 8, fs::SourceInfo(__FILE__, __LINE__));
    else
        return malloc(size);
}

void *SDL_calloc(size_t nmemb, size_t size)
{
    return calloc(nmemb, size);
}

void *SDL_realloc(void *ptr, size_t size)
{
    return realloc(ptr, size);
}

void SDL_free(void *ptr)
{
    if(fs::pSdlArena)
        fs::pSdlArena->free(ptr);
    else
        free(ptr);
}

#endif
