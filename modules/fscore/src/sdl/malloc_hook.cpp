#include "fscore/sdl/malloc_hook.h"
#include "fscore/memory/memory_arena.h"

using namespace fs;

static IArenaAdapter* pSdlArena = nullptr;
void setSdlArena(IArenaAdapter* pArena)
{
    pSdlArena = pArena;
}

void *SDL_malloc(size_t size)
{
    if(pSdlArena)
        return pSdlArena->allocate(size, 8, fs::SourceInfo(__FILE__, __LINE__));
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
    if(pSdlArena)
        pSdlArena->free(ptr);
    else
        free(ptr);
}
