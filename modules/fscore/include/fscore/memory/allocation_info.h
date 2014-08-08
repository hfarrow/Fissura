#ifndef FS_ALLOCATION_INFO_H
#define FS_ALLOCATION_INFO_H

#include "fscore/utils/types.h"

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
}

#endif
