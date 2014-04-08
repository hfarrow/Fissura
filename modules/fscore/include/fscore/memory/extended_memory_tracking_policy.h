#ifndef FS_EXTENDED_MEMORY_TRACKING_POLICY_H
#define FS_EXTENDED_MEMORY_TRACKING_POLICY_H

#include "fscore/debugging/memory.h"
#include "fscore/utils/types.h"
#include "fscore/memory/stl_allocator.h"
#include "fscore/debugging/memory_reporting.h"
#include "fscore/debugging/utils.h"

namespace fs
{
    class AllocationInfo
    {
    public:
        AllocationInfo(const char* fileName, u32 lineNumber, size_t size, u32 id) :
            fileName(fileName),
            lineNumber(lineNumber),
            size(size),
            id(id),
            numFrames(0)
        {        
        }
        
        const char* fileName;
        const u32 lineNumber;
        const size_t size;
        const u32 id;

        static const size_t maxStackFrames = 10;
        void* frames[maxStackFrames];
        size_t numFrames;
    };

    using AllocationMap = DebugMap<uptr, AllocationInfo>;
    using AllocationMapAlloc = DebugMapAllocator<uptr, AllocationInfo>;
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

        inline size_t getNumAllocations() const { return _profile.numAllocations; }
        inline size_t getAllocatedSize() const { return _profile.usedSize; }
        void reset();

        template<typename Arena>
        void logMemoryReport(Arena& arena);

    protected:
        u32 _nextId;
        MemoryProfileExtended _profile;
    };

    template<typename Arena>
    void ExtendedMemoryTracking::logMemoryReport(Arena& arena)
    {
        memory::logArenaReport(arena, *this);

        for(auto pair : *_profile.pAllocationMap)
        {
            uptr ptr = pair.first;
            AllocationInfo& info = pair.second;
            FS_PRINT("    Allocation(" << info.id << "): " 
                     << (void*)ptr << " | " 
                     << info.size << " | " 
                     << info.fileName << ":" << info.lineNumber << " | ");
        }
    }

    class FullMemoryTracking : public ExtendedMemoryTracking
    {
    public: 
        void onAllocation(void* ptr, size_t size, size_t alignment, const SourceInfo& info);

        template<typename Arena>
        void logMemoryReport(Arena& arena);
    };

    template<typename Arena>
    void FullMemoryTracking::logMemoryReport(Arena& arena)
    {
        memory::logArenaReport(arena, *this);

        for(auto pair : *_profile.pAllocationMap)
        {
            uptr ptr = pair.first;
            AllocationInfo& info = pair.second;
            FS_PRINT("    Allocation(" << info.id << "): " 
                     << (void*)ptr << " | " 
                     << info.size << " | " 
                     << info.fileName << ":" << info.lineNumber << " | ");
            
            FS_PRINT("    Stack Trace:");
            FS_PRINT(StackTraceUtil::getCaller(info.frames, info.numFrames, 2));
        }
    }
}

#endif
