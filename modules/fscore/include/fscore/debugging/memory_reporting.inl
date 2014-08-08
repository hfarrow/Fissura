#ifndef FS_MEMORY_REPORTING_INL
#define FS_MEMORY_REPORTING_INL

#include "fscore/debugging/memory_reporting.h"
#include "fscore/utils/types.h"
#include "fscore/debugging/assert.h"
#include "fscore/memory/allocators/stl_allocator.h"
#include "fscore/debugging/memory.h"

namespace fs
{
    namespace memory
    {
        template<class Arena, class MemoryTrackingPolicy>
        const SharedPtr<ArenaReport> generateArenaReport(Arena& arena, MemoryTrackingPolicy& tracker)
        {
            auto report = std::allocate_shared<ArenaReport>(DebugStlAllocator<ArenaReport>());
            generateArenaReport(*report, arena, tracker);
            report->noTracking = false;
            return report;
        }

        template<class Arena, class MemoryTrackingPolicy>
        void generateArenaReport(ArenaReport& report, Arena& arena, MemoryTrackingPolicy& tracker)
        {
            report.arenaName = arena.getName();
            report.numOfAllocations = tracker.getNumAllocations();
            report.virtualSize = arena.getVirtualSize();
            report.physicalSize = arena.getPhysicalSize();
            report.used = arena.getTotalUsedSize();
            report.allocated = arena.getAllocatedSize();
            report.wasted = arena.getTotalUsedSize() - tracker.getAllocatedSize();
            report.hasStackTrace = false;
        }

        template<class Arena, class MemoryTrackingPolicy>
        SharedPtr<ArenaReport> generateExtendedArenaReport(Arena& arena, MemoryTrackingPolicy& tracker)
        {
            auto report = generateArenaReport(arena, tracker);
            generateExtendedArenaReport(*report, arena, tracker);
            return report;
        }

        template<class Arena, class MemoryTrackingPolicy>
        void generateExtendedArenaReport(ArenaReport& report, Arena& arena, MemoryTrackingPolicy& tracker)
        {
            (void)arena;

            // TODO: copy the allocation map
            report.pAllocationMap = tracker.getAllocationMap();
        }

        template<class Arena, class MemoryTrackingPolicy>
        SharedPtr<ArenaReport> generateFullArenaReport(Arena& arena, MemoryTrackingPolicy& tracker)
        {
            auto report = generateExtendedArenaReport(arena, tracker);
            report->hasStackTrace = true;
            return report;
        }
    }
}

#endif
