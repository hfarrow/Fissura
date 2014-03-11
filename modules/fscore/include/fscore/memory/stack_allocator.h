#ifndef FS_STACK_ALLOCATOR_H
#define FS_STACK_ALLOCATOR_H

#include <functional>

#include "fscore/utils/types.h"
#include "fscore/debugging/assert.h"

namespace fs
{
    class PageAllocator;

    class StackAllocator
    {
    public:
        template<typename BackingAllocator = PageAllocator>
        explicit StackAllocator(size_t size);
        StackAllocator(void* start, void* end);
        ~StackAllocator();

        void* allocate(size_t size, size_t alignment, size_t offset);
        void free(void* ptr);
        
        inline void reset()
        {
            _current = _start;
            _lastUserPtr = _current;
        }

        inline size_t getAllocatedSpace()
        {
            return _current - _start;
        }
    
    private:
        uptr _start;
        uptr _end;
        uptr _current;
        uptr _lastUserPtr;
        std::function<void()> _deleter;
    };

    // Templated constructor implementation
    template<typename BackingAllocator>
    StackAllocator::StackAllocator(size_t size)
    {
        FS_ASSERT(size > 0);

        static BackingAllocator allocator;
        void* ptr = allocator.allocate(size);
        FS_ASSERT_MSG(ptr, "Failed to allocate pages for LinearAllocator");

        _start = (uptr)ptr;
        _end = _start + size;
        _current = _start;
        _lastUserPtr = _start;

        _deleter = std::function<void()>([this](){allocator.free((void*)_start, _end - _start);});
    }
}

#endif

