#ifndef FS_LINEAR_ALLOCATOR_H
#define FS_LINEAR_ALLOCATOR_H

#include "fscore/utils/types.h"
#include "fscore/debugging/assert.h"

namespace fs
{
    class PageAllocator;

    class LinearAllocator
    {
    public:
        explicit LinearAllocator(size_t size);
        LinearAllocator(void* start, void* end);
        ~LinearAllocator();

        void* allocate(size_t  size, size_t alignment, size_t offset);

        inline void free(void* ptr)
        {
            (void)ptr;
            FS_ASSERT(!"LinearAllocator can not free allocations");
        }

        inline void reset()
        {
            _current = _start;
        }

    private:
        uptr _start;
        uptr _end;
        uptr _current;
        PageAllocator* _pBackingAllocator;
    };
}

#endif
