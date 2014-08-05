#ifndef FS_MEMORY_TRACKING_POLICY_H
#define FS_MEMORY_TRACKING_POLICY_H

#include <map>
#include <memory>

#include "fscore/utils/types.h"
#include "fscore/debugging/assert.h"
#include "fscore/debugging/memory.h"
#include "fscore/debugging/memory_reporting.h"
#include "fscore/memory/source_info.h"

namespace fs
{
    class SimpleMemoryTracking;
    using DebugMemoryTrackingPolicy = SimpleMemoryTracking;

    class MemoryProfileSimple : Uncopyable
    {
    public:
        size_t numAllocations = 0;
        size_t usedSize = 0;
    };

    class NoMemoryTracking
    {
    public:
        inline void onAllocation(void*, size_t, size_t, const SourceInfo&) const {}
        inline void onDeallocation(void*, size_t) const {}
        inline size_t getNumAllocations() const {return 0;}
        inline size_t getAllocatedSize() const {return 0;}
        inline void reset() {}

        template<typename Arena>
        inline void logMemoryReport(Arena& arena)
        {
            FS_PRINT(arena.getName() << " does not use memory tracking.");
        }
    };

    class SimpleMemoryTracking
    {
    public:
        inline void onAllocation(void*, size_t size, size_t, const SourceInfo&)
        {
            _profile.numAllocations++;
            _profile.usedSize += size;
        }

        inline void onDeallocation(void*, size_t size)
        {
            FS_ASSERT_MSG(_profile.numAllocations > 0, "This arena has no current allocations and therefore cannot free.");
            _profile.numAllocations--;
            _profile.usedSize -= size;
        }

        inline size_t getNumAllocations() const {return _profile.numAllocations;}
        inline size_t getAllocatedSize() const {return _profile.usedSize;}

        inline void reset()
        {
            _profile.numAllocations = 0;
            _profile.usedSize = 0;
        }

        template<typename Arena>
        void logMemoryReport(Arena& arena)
        {
            memory::logArenaReport(arena, *this);
        }

    protected:
        MemoryProfileSimple _profile;
    };
}

#endif
