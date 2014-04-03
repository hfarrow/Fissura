#ifndef FS_LINEAR_ALLOCATOR_H
#define FS_LINEAR_ALLOCATOR_H

#include <functional>

#include "fscore/utils/types.h"
#include "fscore/debugging/assert.h"

namespace fs
{
    class PageAllocator;

    class LinearAllocator
    {
    public:
        template<typename BackingAllocator = PageAllocator>
        explicit LinearAllocator(size_t size);

        LinearAllocator(void* start, void* end);

        ~LinearAllocator();

        void* allocate(size_t  size, size_t alignment, size_t offset);

        inline void free(void* ptr)
        {
            (void)ptr;
            FS_ASSERT(!"LinearAllocator can not free allocations");
        }

        inline void reset() { _current = _start; }
        inline size_t getTotalUsedSize() const { return _current - _start; }
        inline size_t getVirtualSize() const {  return _end - _start; }
        inline size_t getPhysicalSize() const { return _end - _start; }

    private:
        uptr _start;
        uptr _end;
        uptr _current;
        std::function<void()> _deleter;
    };
    
    // Templated constructor implementation
    template<typename BackingAllocator>
    LinearAllocator::LinearAllocator(size_t size)
    {
        FS_ASSERT(size > 0);

        static BackingAllocator allocator;
        void* ptr = allocator.allocate(size);
        FS_ASSERT_MSG(ptr, "Failed to allocate pages for LinearAllocator");

        _start = (uptr)ptr;
        _end = _start + size;
        _current = _start;

        _deleter = std::function<void()>([this](){allocator.free((void*)_start, _end - _start);});
    }
}

#endif
