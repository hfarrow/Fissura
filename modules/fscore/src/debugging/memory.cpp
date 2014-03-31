#include "fscore/debugging/memory.h"

using namespace fs;

#ifdef FS_NO_DEFAULT_DEBUG_ARENA
        static DebugArena* pDebugArena = nullptr;
#else
#ifndef FS_DEFAULT_DEBUG_ARENA_SIZE
#define FS_DEFAULT_DEBUG_ARENA_SIZE FS_SIZE_OF_MB * 32
#endif
        static DebugArena debugArena((size_t)(FS_DEFAULT_DEBUG_ARENA_SIZE), "DefaultDebugArena_32mb");
        static DebugArena* pDebugArena = &debugArena;
#endif


void memory::setDebugArena(DebugArena& arena)
{
    FS_ASSERT_MSG(!pDebugArena, "Can only set debug arena once.");
    pDebugArena = &arena;
}

DebugArena* memory::getDebugArena()
{
    FS_ASSERT_MSG(pDebugArena, "Debug arena must be set first.");
    return pDebugArena;
}
