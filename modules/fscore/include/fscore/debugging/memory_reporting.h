#ifndef FS_MEMORY_REPORTING_H
#define FS_MEMORY_REPORTING_H

namespace fs
{
    namespace memory
    {
        template<class Arena, class MemoryTrackingPolicy>
        void logArenaReport(Arena& arena, MemoryTrackingPolicy& memoryTracker);
    }
}

#include "fscore/debugging/memory_reporting.inl"

#endif
