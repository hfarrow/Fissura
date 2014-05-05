#ifndef FS_SDL_MALLOC_HOOK_H
#define FS_SDL_MALLOC_HOOK_H

#include <cstdlib>

#include "fscore/utils/types.h"
#include "fscore/memory/memory_arena.h"

namespace fs
{
    void setSdlArena(fs::IArenaAdapter* pArena);
}

void *SDL_malloc(size_t size);
void *SDL_calloc(size_t nmemb, size_t size);
void *SDL_realloc(void *ptr, size_t size);
void SDL_free(void *ptr);

#endif
