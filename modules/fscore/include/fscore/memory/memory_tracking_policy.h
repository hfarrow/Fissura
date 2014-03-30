#ifndef FS_MEMORY_TRACKING_POLICY_H
#define FS_MEMORY_TRACKING_POLICY_H

#include "fscore/utils/types.h"
#include "fscore/debugging/assert.h"
#include "fscore/memory/source_info.h"

namespace fs
{
    class MemoryProfile : Uncopyable
    {
    public:
        size_t numAllocations = 0;
        size_t usedSize = 0;
    };

    class NoMemoryTracking
    {
    public:
        inline void onAllocation(void*, size_t, size_t, const SourceInfo&) const {}
        inline void onDeallocation(void*, size_t) const {}
        inline size_t getNumAllocations() const {return 0;}
        inline size_t getUsedSize() const {return 0;}
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
            FS_ASSERT_MSG(_profile.numAllocations > 0, "This arena has no current allocations and therfore cannot free.");
            _profile.numAllocations--;
            _profile.usedSize -= size;
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
