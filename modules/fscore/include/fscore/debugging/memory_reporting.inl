#ifndef FS_MEMORY_REPORTING_INL
#define FS_MEMORY_REPORTING_INL

#include "fscore/utils/types.h"
#include "fscore/debugging/assert.h"

namespace fs
{
    namespace memory
    {
        template<class Arena, class MemoryTrackingPolicy>
        void logArenaReport(Arena& arena, MemoryTrackingPolicy& tracker)
        {
            FS_PRINT("logging arena leaks (extended):");
            FS_PRINT("    Arena Name: " << arena.getName());
            FS_PRINT("    Number of Allocations: " << tracker.getNumAllocations());
            FS_PRINT("    Virtual Size:  " << arena.getVirtualSize());
            FS_PRINT("    Physical Size: " << arena.getPhysicalSize());
            FS_PRINT("    Used:      " << arena.getTotalUsedSize());
            FS_PRINT("    Allocated: " << tracker.getAllocatedSize());
            FS_PRINT("    Wasted:    " << arena.getTotalUsedSize() - tracker.getAllocatedSize());
        }
    }
}

#endif
