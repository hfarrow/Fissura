#ifndef FS_MALLOC_ALLOCATOR_H
#define FS_MALLOC_ALLOCATOR_H

#include <functional>
#include <stdio.h>

#include "fscore/utils/types.h"
#include "fscore/debugging/assert.h"

namespace fs
{
    class MallocAllocator
    {
    public:
        MallocAllocator(void* start, void* end)
        {
            (void)start;
            (void)end;
        }

        ~MallocAllocator()
        {

        }

        void* allocate(size_t  size, size_t alignment, size_t offset)
        {
            (void)alignment;
            (void)offset;
            return malloc(size);
        }

        inline void free(void* ptr)
        {
            free(ptr);
        }

        inline void reset() { FS_ASSERT_MSG(false, "MallocAllocator cannot be reset."); }
        inline size_t getTotalUsedSize() const { return 0; }
        inline size_t getVirtualSize() const {  return 0; }
        inline size_t getPhysicalSize() const { return 0; }

    private:
    };
}

#endif

