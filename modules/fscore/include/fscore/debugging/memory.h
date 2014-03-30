#ifndef FS_DEGUG_MEMORY_H
#define FS_DEGUG_MEMORY_H

#include "fscore/utils/types.h"
#include "fscore/debugging/assert.h"
#include "fscore/memory/memory_arena.h"
#include "fscore/memory/heap_allocator.h"
#include "fscore/memory/allocation_policy.h"
#include "fscore/memory/bounds_checking_policy.h"
#include "fscore/memory/memory_tagging_policy.h"
#include "fscore/memory/memory_tracking_policy.h"
#include "fscore/memory/thread_policy.h"

#define FS_SIZE_OF_MB 33554432

namespace fs
{
    using DebugArena = MemoryArena<Allocator<HeapAllocator, AllocationHeaderU32>,
                                   MultiThreadPolicy<MutexPrimitive>,
                                   SimpleBoundsChecking,
                                   NoMemoryTracking,
                                   MemoryTagging>;
    namespace memory
    {
#ifdef FS_NO_DEFAULT_DEBUG_ARENA
        static DebugArena* pDebugArena = nullptr;
#else
#ifndef FS_DEFAULT_DEBUG_ARENA_SIZE
#define FS_DEFAULT_DEBUG_ARENA_SIZE FS_SIZE_OF_MB * 32
#endif
        static DebugArena debugArena((size_t)(FS_DEFAULT_DEBUG_ARENA_SIZE));
        static DebugArena* pDebugArena = &debugArena;
#endif

        void setDebugArena(DebugArena& arena)
        {
            FS_ASSERT_MSG(!pDebugArena, "Can only set debug arena once.");
            pDebugArena = &arena;
        }

        DebugArena* getDebugArena()
        {
            FS_ASSERT_MSG(pDebugArena, "Debug arena must be set first.");
            return pDebugArena;
        }
    }
}

#endif
