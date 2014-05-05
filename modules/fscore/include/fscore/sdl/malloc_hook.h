#ifndef FS_SDL_MALLOC_HOOK_H
#define FS_SDL_MALLOC_HOOK_H

#include "fscore/utils/types.h"

namespace fs
{
    class IArenaAdapter;

    void setSdlArena(IArenaAdapter* pArena);
}
extern "C" {
    void *SDL_malloc(size_t size);
    void *SDL_calloc(size_t nmemb, size_t size);
    void *SDL_realloc(void *ptr, size_t size);
    void SDL_free(void *ptr);
}

#endif
