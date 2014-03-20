#ifndef FS_POOL_ALLOCATOR
#define FS_POOL_ALLOCATOR

#include "fscore/utils/types.h"
#include "fscore/debugging/assert.h"
#include "fscore/memory/utils.h"
#include "fscore/memory/freelist.h"
#include "fscore/memory/policies.h"

namespace fs
{
    class PageAllocator;

    template<typename GrowthPolicy>
    class PoolAllocator
    {

    public:
        // Constructor for Growable Pool only.
        PoolAllocator(size_t initialSize, size_t maxSize, size_t maxElementSize, size_t maxAlignment, size_t offset, size_t growSize);

        // Constructor for NonGrowable Pool only.
        template<typename BackingAllocator = PageAllocator>
        explicit PoolAllocator(size_t size, size_t maxElementSize, size_t maxAlignment, size_t offset);

        // Constructor for NonGrowable Pool only.
        PoolAllocator(void* start, void* end, size_t maxElementSize, size_t maxAlignment, size_t offset);
        
        ~PoolAllocator();

        void* allocate(size_t size, size_t alignment, size_t offset);
        inline void free(void* ptr);
        inline void reset();

    private:
        const size_t _maxElementSize;
        const size_t _maxAlignment;
        const size_t _offset;
        void* _virtualStart;
        void* _virtualEnd;
        void* _physicalEnd;
        Freelist<IndexSize::systemDefault> _freelist;
        size_t _growSize;
        std::function<void()> _deleter;
        GrowthPolicy _growthPolicy;
    };

    using PoolAllocatorGrowable = PoolAllocator<Growable>;
    using PoolAllocatorNonGrowable = PoolAllocator<NonGrowable>;
}

#include "fscore/memory/pool_allocator.inl"

#endif
