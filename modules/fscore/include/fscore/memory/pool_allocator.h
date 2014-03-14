#ifndef FS_POOL_ALLOCATOR
#define FS_POOL_ALLOCATOR

#include "fscore/utils/types.h"
#include "fscore/debugging/assert.h"
#include "fscore/memory/utils.h"
#include "fscore/memory/freelist.h"

namespace fs
{
    class PageAllocator;

    class PoolAllocator
    {
        using FreelistDefault = Freelist<0>;

    public:
        template<typename BackingAllocator = PageAllocator>
        explicit PoolAllocator(size_t size, size_t maxElementSize, size_t maxAlignment, size_t offset);
        PoolAllocator(void* start, void* end, size_t maxElementSize, size_t maxAlignment, size_t offset);
        ~PoolAllocator();

        void* allocate(size_t size, size_t alignment, size_t offset);
        
        inline void free(void* ptr)
        {
            _freelist.release(ptr);
        }

        inline void reset()
        {
            _freelist = FreelistDefault(_start, _end, _maxElementSize, _maxAlignment, _offset);
        }

    private:
        const size_t _maxElementSize;
        const size_t _maxAlignment;
        const size_t _offset;
        void* _start;
        void* _end;
        FreelistDefault _freelist;
        std::function<void()> _deleter;
    };

    // Templated constructor implementation
    template<typename BackingAllocator>
    PoolAllocator::PoolAllocator(size_t size, size_t maxElementSize, size_t maxAlignment, size_t offset) :
        _maxElementSize(maxElementSize),
        _maxAlignment(maxAlignment),
        _offset(offset),
        _freelist()
    {
        FS_ASSERT(size > 0);

        static BackingAllocator allocator;
        void* ptr = allocator.allocate(size);
        FS_ASSERT_MSG(ptr, "Failed to allocate pages for PoolAllocator");

        _start = ptr;
        _end = (void*)((uptr)_start + size);
        _freelist = FreelistDefault(_start, _end, _maxElementSize, _maxAlignment, _offset);

        _deleter = std::function<void()>([this](){allocator.free(_start, (uptr)_end - (uptr)_start);});
    }
}

#endif
