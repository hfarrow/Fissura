#ifndef FS_MEMORY_TRACKING_POLICY_H
#define FS_MEMORY_TRACKING_POLICY_H

#include <map>
#include <memory>

#include "fscore/utils/types.h"
#include "fscore/debugging/assert.h"
#include "fscore/debugging/memory.h"
#include "fscore/memory/source_info.h"
#include "fscore/debugging/arena_report.h"

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
        inline SharedPtr<ArenaReport> generateArenaReport(Arena& arena)
        {
            // This class cannot reference or include DebugArena so we are forced to create the report directly
            // via new instead of within an arena. yuck!
            auto report = SharedPtr<ArenaReport>(new ArenaReport());
            report->arenaName = arena.getName();
            report->noTracking = true;
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
        SharedPtr<ArenaReport> generateArenaReport(Arena& arena)
        {
            // This class cannot reference or include DebugArena so we are forced to create the report directly
            // via new instead of within an arena. yuck!
            auto report = SharedPtr<ArenaReport>(new ArenaReport());
            report->arenaName = arena.getName();
            report->numOfAllocations = getNumAllocations();
            report->virtualSize = arena.getVirtualSize();
            report->physicalSize = arena.getPhysicalSize();
            report->used = arena.getTotalUsedSize();
            report->allocated = arena.getAllocatedSize();
            report->wasted = arena.getTotalUsedSize() - getAllocatedSize();
            report->hasStackTrace = false;
            report->noTracking = false;
        }

    protected:
        MemoryProfileSimple _profile;
    };
}

#endif
