#ifndef FS_ARENA_REPORT_H
#define FS_ARENA_REPORT_H 

#include "fscore/utils/types.h"
#include "fscore/memory/allocation_info.h"

namespace fs
{
    class ArenaReport
    {
        public:
            const char* arenaName;
            size_t numOfAllocations;
            size_t virtualSize;
            size_t physicalSize;
            size_t used;
            size_t allocated;
            size_t wasted;
            SharedPtr<AllocationMap> pAllocationMap;
            bool hasStackTrace;
            bool noTracking;
    };
}

#endif
