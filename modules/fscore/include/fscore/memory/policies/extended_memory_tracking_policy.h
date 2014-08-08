#ifndef FS_EXTENDED_MEMORY_TRACKING_POLICY_H
#define FS_EXTENDED_MEMORY_TRACKING_POLICY_H

#include "fscore/debugging/memory.h"
#include "fscore/utils/types.h"
#include "fscore/memory/allocators/stl_allocator.h"
#include "fscore/debugging/memory_reporting.h"
#include "fscore/debugging/memory_logging.h"
#include "fscore/debugging/utils.h"
#include "fscore/debugging/logger.h"
#include "fscore/memory/allocation_info.h"

namespace fs
{


    class MemoryProfileExtended : public MemoryProfileSimple
    {
    public:
        SharedPtr<AllocationMap> pAllocationMap;
    };

    class ExtendedMemoryTracking
    {
    public:
        ExtendedMemoryTracking();
        void onAllocation(void* ptr, size_t size, size_t alignment, const SourceInfo& info);
        void onDeallocation(void* ptr, size_t size);

        inline size_t getNumAllocations() const { return _profile.numAllocations; }
        inline size_t getAllocatedSize() const { return _profile.usedSize; }
        inline SharedPtr<AllocationMap> getAllocationMap() const { return _profile.pAllocationMap; }
        void reset();

        template<typename Arena>
        SharedPtr<ArenaReport> generateArenaReport(Arena& arena);

    protected:
        u32 _nextId;
        MemoryProfileExtended _profile;
    };

    template<typename Arena>
    SharedPtr<ArenaReport> ExtendedMemoryTracking::generateArenaReport(Arena& arena)
    {
       return memory::generateExtendedArenaReport(arena, *this);
    }

    class FullMemoryTracking : public ExtendedMemoryTracking
    {
    public:
        void onAllocation(void* ptr, size_t size, size_t alignment, const SourceInfo& info);

        template<typename Arena>
        SharedPtr<ArenaReport> generateArenaReport(Arena& arena);
    };

    template<typename Arena>
    SharedPtr<ArenaReport> FullMemoryTracking::generateArenaReport(Arena& arena)
    {
        return memory::generateFullArenaReport(arena, *this);
    }
}

#endif
