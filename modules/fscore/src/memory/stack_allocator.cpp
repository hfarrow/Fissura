#include "fscore/memory/stack_allocator.h"

#include "fscore/debugging/assert.h"
#include "fscore/memory/page_allocator.h"
#include "fscore/memory/utils.h"

using namespace fs;

namespace
{
    static const size_t SIZE_OF_ALLOCATION_OFFSET = sizeof(u32) * 2;
    static_assert(SIZE_OF_ALLOCATION_OFFSET == 8, "Allocation offset has wrong size.");
}

StackAllocator::StackAllocator(void* start, void* end) :
    _deleter(nullptr)
{
    FS_ASSERT(start);
    FS_ASSERT(end);
    FS_ASSERT(start < end);
    _start = (uptr)start;
    _end = (uptr)end;
    _current = _start;
    _lastUserPtr = _start;
}

StackAllocator::~StackAllocator()
{
    if(_deleter)
    {
        _deleter();
    }
}

void* StackAllocator::allocate(size_t size, size_t alignment, size_t offset)
{
    // store the allocation offset infront of the allocation
    size += SIZE_OF_ALLOCATION_OFFSET;
    offset += SIZE_OF_ALLOCATION_OFFSET;

    const uptr oldCurrent = _current;
    
    // offset the pointer first, align it, and then offset it back
    _current = pointerUtil::alignTop(_current + offset, alignment) - offset;

    const u32 headerSize = static_cast<u32>(_current - oldCurrent);
    
    if(_current + size > _end)
    {
        FS_ASSERT(!"StackAllocator out of memory");
        return nullptr;
    }

    union
    {
        void* as_void;
        char* as_char;
        u32* as_u32;
        uptr as_uptr;
    };

    as_char = (char*)_current;
    
    // store lastUserPtr in the first 4 bytes
    // store allocation offset in the second 4 bytes
    const uptr lastUserPtrOffset = _current - _lastUserPtr;
    *as_u32 = lastUserPtrOffset;
    *(as_u32 + 1) = headerSize;
    as_char += SIZE_OF_ALLOCATION_OFFSET;

    void* userPtr = as_void;
    _lastUserPtr = as_uptr;
    _current += size;
    return userPtr;
}

void StackAllocator::free(void* ptr)
{
    FS_ASSERT(ptr);
    FS_ASSERT(ptr == (void*)_lastUserPtr);
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
    _lastUserPtr = as_uptr - lastUserPtrOffset;

    _current = (uptr)ptr - headerSize;
}
