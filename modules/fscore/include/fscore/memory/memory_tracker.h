#ifndef FS_MEMORY_TRACKER_H
#define FS_MEMORY_TRACKER_H

#include "fscore/debugging/assert.h"
#include "fscore/utils/types.h"
#include "fscore/allocators/allocator.h"
#include "fscore/allocators/malloc_allocator.h"
#include "fscore/allocators/stl_allocator.h"

namespace fs
{
    class MemoryTracker : Uncopyable
    {
    public:

        struct AllocatorInfo
        {
            Allocator* pAllocator;
            size_t usedMemory;
            size_t numAllocations;
            size_t peakUsedMemory;
            size_t peakNumAllocations;
        };

        struct Report
        {
            std::shared_ptr<Vector<AllocatorInfo>> allocators;
        };


        MemoryTracker();
        ~MemoryTracker();

        void registerAllocator(Allocator* pAllocator);
        void unregisterAllocator(Allocator* pAllocator);
        void update();
        const Report generateReport();

        typedef Map<Allocator*, AllocatorInfo> AllocatorMap;
        typedef MapAllocator<Allocator*, AllocatorInfo> AllocatorMapAllocator;
        UniquePtr<AllocatorMap> _pAllocatorMap;
        AllocatorMapAllocator _allocatorMapAllocator;
        StlAllocator<Vector<AllocatorInfo>> _allocatorVectorAllocator;
    };
}

#endif
