#ifndef FS_MEMORY_REPORTING_H
#define FS_MEMORY_REPORTING_H

#include "fscore/types.h"
#include "fsmem/debug/arena_report.h"

namespace fs
{
    namespace memory
    {
        template<class Arena, class MemoryTrackingPolicy>
        const SharedPtr<ArenaReport> generateArenaReport(Arena& arena, MemoryTrackingPolicy& tracker);

        template<class Arena, class MemoryTrackingPolicy>
        void generateArenaReport(ArenaReport& report, Arena& arena, MemoryTrackingPolicy& tracker);

        template<class Arena, class MemoryTrackingPolicy>
        SharedPtr<ArenaReport> generateExtendedArenaReport(Arena& arena, MemoryTrackingPolicy& tracker);

        template<class Arena, class MemoryTrackingPolicy>
        void generateExtendedArenaReport(ArenaReport& report, Arena& arena, MemoryTrackingPolicy& tracker);

        template<class Arena, class MemoryTrackingPolicy>
        SharedPtr<ArenaReport> generateFullArenaReport(Arena& arena, MemoryTrackingPolicy& tracker);
    }
}

#include "fsmem/debug/memory_reporting.inl"

#endif
