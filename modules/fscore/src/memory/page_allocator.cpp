#include "fscore/memory/page_allocator.h"

#include <sys/mman.h>
#include <boost/format.hpp>

#include "fscore/debugging/assert.h"

using namespace fs;

PageAllocator::PageAllocator()
{

}

PageAllocator::~PageAllocator()
{

}

void* PageAllocator::allocate(size_t size, size_t alignment, size_t offset)
{
    FS_ASSERT(alignment == 0);
    FS_ASSERT(offset == 0);

    void* userPtr = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if(userPtr == MAP_FAILED)
    {
        FS_ASSERT_MSG_FORMATTED(false,
                                boost::format("Failed to mmap requested size %u. errno: %i")
                                % size % errno);
        return nullptr;
    }

    return userPtr;
}

void PageAllocator::free(void* ptr, size_t size)
{
    FS_ASSERT(ptr);
    FS_ASSERT(size > 0);

    if(munmap(ptr, size))
    {
        FS_ASSERT_MSG_FORMATTED(false,
                                boost::format("Failed to munmap pointer %p of size %u. errno: %i")
                                % ptr % size % errno);
    }
}
