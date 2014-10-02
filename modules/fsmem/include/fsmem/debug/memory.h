#ifndef FS_DEGUG_MEMORY_H
#define FS_DEGUG_MEMORY_H

#include "fscore/types.h"
#include "fscore/assert.h"
#include "fsmem/memory_arena.h"
#include "fsmem/allocators/heap_allocator.h"
#include "fsmem/policies/allocation_policy.h"
#include "fsmem/policies/bounds_checking_policy.h"
#include "fsmem/policies/memory_tagging_policy.h"
#include "fsmem/policies/memory_tracking_policy.h"
#include "fsmem/policies/thread_policy.h"

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
