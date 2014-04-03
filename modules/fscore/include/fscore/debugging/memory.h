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
    using DebugArena = MemoryArena<DebugAllocationPolicy,
                                   DebugThreadPolicy,
                                   DebugBoundsCheckingPolicy,
                                   DebugMemoryTrackingPolicy,
                                   DebugMemoryTaggingPolicy>;

    namespace memory
    {
        void setDebugArena(DebugArena& arena);
        DebugArena* getDebugArena();
    }
}

#endif