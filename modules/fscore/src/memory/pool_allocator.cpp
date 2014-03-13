#include "fscore/memory/pool_allocator.h"

#include "fscore/memory/utils.h"
#include "fscore/debugging/assert.h"

#include <iostream>

using namespace fs;

Freelist::Freelist(void* start, void* end, size_t elementSize, size_t alignment, size_t offset)
{
    FS_ASSERT(alignment > 0);

    // Freelist is stored as pointers which require the element size to
    // be at least the size of a pointer.
    if(elementSize < sizeof(void*))
    {
        elementSize = sizeof(void*);
    }
    
    // Ensure that the first free slot is starting off aligned.
    const uptr alignedStart = pointerUtil::alignTop((uptr)start + offset, alignment) - offset;

    // Add padding to elementSize to ensure all following slots are also aligned.
    const size_t slotSize = pointerUtil::roundUp(elementSize, alignment);
    FS_ASSERT(slotSize >= elementSize);
    
    // Calculate total available memory after alignment is factored in.
    const size_t size = (uptr)end - alignedStart;
    const u32 numElements = size / slotSize;

    union
    {
        void* as_void;
        Freelist* as_self;
        uptr as_uptr;
    };

    as_uptr = alignedStart;
    _next = as_self;
    as_uptr += slotSize;

    // initialize the free list. Each element points to the next free element.
    Freelist* runner = _next;
    for(size_t i = 1; i < numElements; ++i)
    {
        runner->_next = as_self;
        runner = as_self;
        as_uptr += slotSize;
    }

    runner->_next = nullptr;
}
