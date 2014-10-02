#include "fsmem/debug/memory_logging.h"

#include "fscore/types.h"
#include "fscore/log.h"
#include "fscore/assert.h"
#include "fsmem/allocators/stl_allocator.h"
#include "fsmem/debug/arena_report.h"
#include "fsmem/debug/utils.h"

using namespace fs;

void memory::logArenaReport(const SharedPtr<ArenaReport> report)
{
    if(!report)
    {
        FS_ASSERT(!"Cannot print null report");
        return;
    }


    FS_CORE_INFO("logging arena report:");
    FS_CORE_INFOF("    Arena Name: %s", report->arenaName);

    if(!report->noTracking)
    {
        FS_CORE_INFOF("    Number of Allocations: %u", report->numOfAllocations);
        FS_CORE_INFOF("    Virtual Size:  %u", report->virtualSize);
        FS_CORE_INFOF("    Physical Size: %u", report->physicalSize);
        FS_CORE_INFOF("    Used:      %u", report->used);
        FS_CORE_INFOF("    Allocated: %u", report->allocated);
        FS_CORE_INFOF("    Wasted:    %u", report->wasted);
    }
    else
    {
        FS_CORE_INFO("    >>> No Tracking Information <<<");
    }

    if(report->pAllocationMap)
    {
        for(auto pair : *report->pAllocationMap)
        {
            uptr ptr = pair.first;
            AllocationInfo& info = pair.second;
            FS_CORE_INFOF("    Allocation(%u): %p | %u | %s:%u | "
                    , info.id
                    , (void*)ptr
                    , info.size
                    , info.fileName
                    , info.lineNumber);

            if(report->hasStackTrace)
            {
                FS_CORE_INFOF("    Stack Trace:\n%s", StackTraceUtil::getCaller(info.frames, info.numFrames, 0));
            }
            else
            {
                FS_CORE_INFO("    >>> No Stack Trace Information <<<");
            }
        }
    }
    else
    {
        FS_CORE_INFO("    >>> No Allocation Info <<<");
    }
}
