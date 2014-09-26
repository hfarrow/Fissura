#include "fscore/debugging/memory_logging.h"

#include "fscore/utils/types.h"
#include "fscore/debugging/logger.h"
#include "fscore/debugging/arena_report.h"
#include "fscore/debugging/utils.h"

using namespace fs;

void memory::logArenaReport(const SharedPtr<ArenaReport> report)
{
    if(!report)
    {
        FS_ASSERT(!"Cannot print null report");
        return;
    }


    FS_INFO("logging arena report:");
    FS_INFOF("    Arena Name: %1%", report->arenaName);

    if(!report->noTracking)
    {
        FS_INFOF("    Number of Allocations: %1%", report->numOfAllocations);
        FS_INFOF("    Virtual Size:  %1%", report->virtualSize);
        FS_INFOF("    Physical Size: %1%", report->physicalSize);
        FS_INFOF("    Used:      %1%", report->used);
        FS_INFOF("    Allocated: %1%", report->allocated);
        FS_INFOF("    Wasted:    %1%", report->wasted);
    }
    else
    {
        FS_INFO("    >>> No Tracking Information <<<");
    }

    if(report->pAllocationMap)
    {
        for(auto pair : *report->pAllocationMap)
        {
            uptr ptr = pair.first;
            AllocationInfo& info = pair.second;
            FS_INFOF("    Allocation(%1%): %2% | %3% | %4%:%5% | "
                    , info.id
                    , (void*)ptr
                    , info.size
                    , info.fileName
                    , info.lineNumber);

            if(report->hasStackTrace)
            {
                FS_INFOF("    Stack Trace:\n%1%", StackTraceUtil::getCaller(info.frames, info.numFrames, 0));
            }
            else
            {
                FS_INFO("    >>> No Stack Trace Information <<<");
            }
        }
    }
    else
    {
        FS_INFO("    >>> No Allocation Info <<<");
    }
}
