#ifndef FS_HEAP_ALLOCATOR_H
#define FS_HEAP_ALLOCATOR_H

#include <functional>

#include "fscore/utils/types.h"
#include "fscore/debugging/assert.h"
#include "fscore/memory/malloc.h"

namespace fs
{
    class PageAllocator;

    class HeapAllocator
    {
    public:
        template<typename BackingAllocator = PageAllocator>
        explicit HeapAllocator(size_t size);

        HeapAllocator(void* start, void* end);

        ~HeapAllocator();

        void* allocate(size_t size, size_t alignment, size_t offset);
        void free(void* ptr);
        void reset();
        inline void purge() {}
        size_t getTotalUsedSize();

    private:
        void* _start;
        void* _end;
        std::function<void()> _deleter;
        mspace _mspace;

        void createHeap();
    };

    // Templated constructor implementation
    template<typename BackingAllocator>
    HeapAllocator::HeapAllocator(size_t size)
    {
        FS_ASSERT(size > 0);

        static BackingAllocator allocator;
        void* ptr = allocator.allocate(size);
        FS_ASSERT_MSG(ptr, "Failed to allocate pages for HeapAllocator");

        _start = ptr;
        _end = (void*)((uptr)_start + size);
        createHeap();

        _deleter = std::function<void()>([this](){allocator.free((void*)_start, (uptr)_end - (uptr)_start);});
    }
}

#endif
