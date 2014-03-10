#include "fscore/memory/page_allocator.h"

#include <sys/mman.h>
#include <boost/format.hpp>

#include "fscore/debugging/assert.h"
#include "fscore/memory/utils.h"


using namespace fs;

void* PageAllocator::allocate(size_t size, size_t alignment, size_t offset)
{
    FS_ASSERT(alignment == 0);
    FS_ASSERT(offset == 0);

    return PagedMemoryUtil::commitMemory(size);
}

void PageAllocator::free(void* ptr, size_t size)
{
    FS_ASSERT(ptr);
    FS_ASSERT(size > 0);
    
    PagedMemoryUtil::freeMemory(ptr, size);
}
