#ifndef FS_MEMORY_TRACKING_POLICY_H
#define FS_MEMORY_TRACKING_POLICY_H

#include <map>

#include "fscore/utils/types.h"
#include "fscore/debugging/assert.h"
#include "fscore/memory/source_info.h"
#include "fscore/memory/heap_allocator.h"
#include "fscore/memory/allocation_policy.h"
#include "fscore/memory/bounds_checking_policy.h"
#include "fscore/memory/memory_tagging_policy.h"
#include "fscore/memory/memory_tracking_policy.h"
#include "fscore/memory/thread_policy.h"
#include "fscore/memory/memory_arena.h"

namespace fs
{
    class SimpleMemoryTracking;
    using DebugMemoryTrackingPolicy = SimpleMemoryTracking;

    class AllocationInfo
    {
    public:
        AllocationInfo(const char* fileName, u32 lineNumber, uptr* stackTrace, size_t stackTraceSize) :
            fileName(fileName),
            lineNumber(lineNumber),
            stackTrace(stackTrace),
            stackTraceSize(stackTraceSize)
        {        
        }

        const char* fileName;
        const u32 lineNumber;
        const uptr* stackTrace;
        const size_t stackTraceSize;
    };

    class MemoryProfile : Uncopyable
    {
        // We cannot use the main DebugArena definition because it would
        // introduce a circular dependencies between /debugging/memory.h and
        // policy headers. The Debug<Policy> typedefs should be changed
        // instead of this typedef or the one in memory.h
        // Must match fscore/debugging/memory.h -> DebugArena typedef
        using DebugArena = MemoryArena<DebugAllocationPolicy,
                                  DebugThreadPolicy,
                                  DebugBoundsCheckingPolicy,
                                  DebugMemoryTrackingPolicy,
                                  DebugMemoryTaggingPolicy>;
    public:
        size_t numAllocations = 0;
        size_t usedSize = 0;
        std::map<uptr, AllocationInfo, std::less<uptr>, DebugArena>* allocations;

    };

    class NoMemoryTracking
    {
    public:
        inline void onAllocation(void*, size_t, size_t, const SourceInfo&) const {}
        inline void onDeallocation(void*, size_t) const {}
        inline size_t getNumAllocations() const {return 0;}
        inline size_t getUsedSize() const {return 0;}
        inline void reset() {}
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
        
        inline size_t getUsedSize() const {return _profile.usedSize;}

        inline void reset()
        {
            _profile.numAllocations = 0;
            _profile.usedSize = 0;
        }

    private:
        MemoryProfile _profile;
    };

    class ExtendedMemoryTracking : public SimpleMemoryTracking
    {
    public:
        inline void onAllocation(void* ptr, size_t size, size_t alignment, const SourceInfo& info)
        {
            SimpleMemoryTracking::onAllocation(ptr, size, alignment, info);
        }

        inline void onDeallocation(void* ptr, size_t size)
        {
            SimpleMemoryTracking::onDeallocation(ptr, size);
        }
    };

    // TODO: FullMemoryTracking

}

#endif
