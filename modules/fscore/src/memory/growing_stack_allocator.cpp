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
    _lastUserPtr(_virtualStart)

{
    FS_ASSERT_MSG(growSize % VirtualMemory::getPageSize() == 0 && growSize != 0,
                  "growSize should be a multiple of page size.");
}

void* GrowingStackAllocator::allocate(size_t size, size_t alignment, size_t offset)
{
    size += SIZE_OF_ALLOCATION_OFFSET;
    offset += SIZE_OF_ALLOCATION_OFFSET;

    const uptr oldCurrent = _physicalCurrent;
    _physicalCurrent = pointerUtil::alignTop(_physicalCurrent + offset, alignment) - offset;
    const u32 headerSize = static_cast<u32>(_physicalCurrent - oldCurrent) + SIZE_OF_ALLOCATION_OFFSET;

    // is there enough physical memory left?
    if(_physicalCurrent + size > _physicalEnd)
    {
        // out of physical memory. If there is still address space left from 
        // what was reserved previously then commit another chunk to physical memory.
        const size_t neededPhysicalSize = bitUtil::roundUpToMultiple(size, _growSize);
        if(_physicalEnd + neededPhysicalSize > _virtualEnd)
        {
            FS_ASSERT(!"GrowingStackAllocator out of memory");
            return nullptr;
        }

        VirtualMemory::allocatePhysicalMemory((void*)_physicalEnd, neededPhysicalSize);
        _physicalEnd += neededPhysicalSize;
    }

    union
    {
        void* as_void;
        char* as_char;
        u32* as_u32;
        uptr as_uptr;
    };

    as_char = (char*)_physicalCurrent;

    // store lastUserPtr in the first 4 bytes
    // store allocation offset in the second 4 bytes
    const uptr lastUserPtrOffset = _physicalCurrent - _lastUserPtr;
    *as_u32 = lastUserPtrOffset;
    *(as_u32 + 1) = headerSize;
    as_char += SIZE_OF_ALLOCATION_OFFSET;

    void* userPtr = as_void;
    _lastUserPtr = as_uptr;
    _physicalCurrent += size;

    return userPtr;
}

void GrowingStackAllocator::free(void* ptr)
{
    FS_ASSERT(ptr);
    FS_ASSERT((uptr)ptr == _lastUserPtr);

    union
    {
        void* as_void;
        char* as_char;
        u32* as_u32;
        uptr as_uptr;
    };

    as_void = ptr;

    as_char -= SIZE_OF_ALLOCATION_OFFSET;
    const u32 lastUserPtrOffset = *as_u32;
    const u32 headerSize = *(as_u32 + 1);
    FS_ASSERT(headerSize >= SIZE_OF_ALLOCATION_OFFSET);

    _lastUserPtr = as_uptr - lastUserPtrOffset;
    _physicalCurrent = (uptr)ptr - headerSize;
}

void GrowingStackAllocator::purge()
{
    // Make sure we free from a page aligned location.
    uptr addressToFree = pointerUtil::alignTop(_physicalCurrent, _growSize);

    const size_t sizeToFree = _physicalEnd - addressToFree;
    VirtualMemory::freePhysicalMemory((void*)addressToFree, sizeToFree);
}
