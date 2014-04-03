#ifndef FS_EXTENDED_MEMORY_TRACKING_POLICY_H
#define FS_EXTENDED_MEMORY_TRACKING_POLICY_H

#include "fscore/debugging/memory.h"
#include "fscore/utils/types.h"
#include "fscore/memory/stl_allocator.h"

namespace fs
{
    class AllocationInfo
    {
    public:
        AllocationInfo(const char* fileName, u32 lineNumber, size_t size, u32 id, uptr* stackTrace, size_t stackTraceSize) :
            fileName(fileName),
            lineNumber(lineNumber),
            size(size),
            id(id),
            stackTrace(stackTrace),
            stackTraceSize(stackTraceSize)
        {        
        }
        
        const char* fileName;
        const u32 lineNumber;
        const size_t size;
        const u32 id;
        const uptr* stackTrace;
        const size_t stackTraceSize;
    };

    using AllocationMap = Map<uptr, AllocationInfo, DebugArena>;
    using AllocationMapAlloc = StlAllocator<AllocationMap, DebugArena>;
    using AllocationMapPair = std::pair<uptr, AllocationInfo>;

    class MemoryProfileExtended : public MemoryProfileSimple
    {
    public:
        SharedPtr<AllocationMap> pAllocationMap;
    };

    class ExtendedMemoryTracking
    {
    public:
        ExtendedMemoryTracking();
        void onAllocation(void* ptr, size_t size, size_t alignment, const SourceInfo& info);
        void onDeallocation(void* ptr, size_t size);

        inline size_t getNumAllocations() const {return _profile.numAllocations;}        
        inline size_t getUsedSize() const {return _profile.usedSize;}
        void reset();

        template<typename Arena>
        void logMemoryReport(Arena& arena);


    private:
        u32 _nextId;
        MemoryProfileExtended _profile;
    };

    template<typename Arena>
    void ExtendedMemoryTracking::logMemoryReport(Arena& arena)
    {
        // TODO: change to LOG instead of PRINT
        FS_PRINT("logging arena leaks:");
        FS_PRINT("    Arena Name: " << arena.getName());
        FS_PRINT("    Number of Allocations: " << getNumAllocations());
        FS_PRINT("    Arena Size: " << arena.getMaxSize() );
        FS_PRINT("    Used:      " << arena.getTotalUsedSize() );
        FS_PRINT("    Allocated: " << getUsedSize() );
        FS_PRINT("    Wasted:    " << arena.getTotalUsedSize() - getUsedSize());

        for(auto pair : *_profile.pAllocationMap)
        {
            uptr ptr = pair.first;
            AllocationInfo& info = pair.second;
            FS_PRINT("    Allocation( " << info.id << "): " << (void*)ptr << " | " << info.size << " | " << info.fileName << ":" << info.lineNumber << " | ");
        }
    }

    // TODO: FullMemoryTracking
}

#endif
