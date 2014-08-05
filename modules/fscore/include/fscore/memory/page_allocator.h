#ifndef FS_PAGE_ALLOCATOR_H
#define FS_PAGE_ALLOCATOR_H

#include "fscore/debugging/assert.h"
#include "fscore/utils/types.h"
#include "fscore/utils/platforms.h"

namespace fs
{
    class PageAllocator
    {
    public:
        PageAllocator(){}
        ~PageAllocator(){}

        void* allocate(size_t size, size_t alignment = 0, size_t offset = 0);
        void free(void* ptr, size_t size);

        void free(void* ptr)
        {
            (void)ptr;
            FS_ASSERT(!"PageAllocator cannot free pages without being passed the size.");
        }

        inline void reset()
        {
            FS_ASSERT(!"PageAllocator cannot reset allocated pages");
        }

        inline void purge() {}
        inline size_t getTotalUsedSize() const { return 0;}
        inline size_t getVirtualSize() const { return 0; }
        inline size_t getPhysicalSize() const { return 0; }
    };
}



#endif
