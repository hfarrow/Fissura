#include "fscore/memory/growing_stack_allocator.h"

#include "fscore/debugging/assert.h"
#include "fscore/memory/utils.h"

using namespace fs;

namespace
{
    static const size_t SIZE_OF_ALLOCATION_OFFSET = sizeof(u32) * 2;
    static_assert(SIZE_OF_ALLOCATION_OFFSET == 8, "Allocation offset has wrong size.");
}

GrowingStackAllocator::GrowingStackAllocator(size_t maxSizeInBytes, size_t growSize) :
    _virtualStart((uptr)VirtualMemory::reserveAddressSpace(maxSizeInBytes)),
    _virtualEnd(_virtualStart + maxSizeInBytes),
    _physicalCurrent(_virtualStart),
    _physicalEnd(_virtualStart),
    _growSize(growSize),
    _lastUserPtr(0)

{
    FS_ASSERT_MSG(growSize % VirtualMemory::getPageSize() == 0,
                  "growSize should be a multiple of page size.");

}

void* GrowingStackAllocator::allocate(size_t size, size_t alignment, size_t offset)
{
    return nullptr;
}

void GrowingStackAllocator::free(void* ptr)
{
    FS_ASSERT((uptr)ptr == _lastUserPtr);
}
