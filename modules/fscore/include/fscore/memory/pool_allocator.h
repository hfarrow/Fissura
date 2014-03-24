#ifndef FS_POOL_ALLOCATOR
#define FS_POOL_ALLOCATOR

#include "fscore/utils/types.h"
#include "fscore/debugging/assert.h"
#include "fscore/memory/utils.h"
#include "fscore/memory/freelist.h"
#include "fscore/memory/policies.h"

namespace fs
{
    using PoolFreelist = Freelist<IndexSize::systemDefault>;

    class PageAllocator;

    template<typename GrowthPolicy, size_t maxElementSize, size_t maxAlignment, size_t offset, size_t growSize>
    class PoolAllocator
    {

    public:
        // Constructor for Growable Pool only.
        // initialSize, maxSize, and growSize are bytes and not num of elements.
        PoolAllocator(size_t initialSize, size_t maxSize);

        // Constructor for NonGrowable Pool only.
        template<typename BackingAllocator = PageAllocator>
        explicit PoolAllocator(size_t size);

        // Constructor for NonGrowable Pool only.
        PoolAllocator(void* start, void* end);
        
        ~PoolAllocator();

        void* allocate(size_t size, size_t alignment, size_t userOffset);
        inline void free(void* ptr);
        inline void reset();

        // Purge not implemented currently. In order to purge, book keeping info would need to be added to
        // track all empty pages. Alternatively, purge could walk the free list marking all free slots
        // and then loop over marked slots to determine if a page is empty. The first option would make
        // allocating a freeing slower and the second option would make purge an expesnive
        // operation that should be used sparingly (ie, during level unload or other loading screens).
        // Another complication of purging a pool allocator is that it would leave holes of unallocated
        // memory. This means a list of unallocated pages would need to be kept and reallocated when needed.
        // Overall purge would greatly increase the complexity of a pool allocator. A better solutions would
        // be to reset the allocator when possible (ie, between levels).
        // void purge(){};

    private:
        void* _virtualStart;
        void* _virtualEnd;
        void* _physicalEnd;
        size_t _maxElementSize;
        PoolFreelist _freelist;
        std::function<void()> _deleter;
        GrowthPolicy _growthPolicy;
    };

    //using PoolAllocatorGrowable = PoolAllocator<Growable>;
    //using PoolAllocatorNonGrowable = PoolAllocator<NonGrowable>;
}

#include "fscore/memory/pool_allocator.inl"

#endif
