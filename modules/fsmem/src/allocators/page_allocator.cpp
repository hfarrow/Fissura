#include "fsmem/allocators/page_allocator.h"

#include <sys/mman.h>
#include <boost/format.hpp>

#include "fscore/assert.h"
#include "fsmem/utils.h"


using namespace fs;

void* PageAllocator::allocate(size_t size, size_t alignment, size_t offset)
{
    FS_ASSERT(alignment == 0);
    FS_ASSERT(offset == 0);

    return VirtualMemory::allocatePhysicalMemory(size);
}

void PageAllocator::free(void* ptr, size_t size)
{
    FS_ASSERT(ptr);
    FS_ASSERT(size > 0);

    VirtualMemory::releaseAddressSpace(ptr, size);
}
