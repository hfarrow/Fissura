#include "fscore/memory/pool_allocator.h"

using namespace fs;


// TODO: take alignment and offset into account.
Freelist::Freelist(void* start, void* end, size_t elementSize, size_t alignment, size_t offset)
{
    union
    {
        void* as_void;
        char* as_char;
        Freelist* as_self;
        uptr as_uptr;
    };

    const size_t size = (uptr)end - (uptr)start;
    const u32 numElements = size / elementSize;
    
    // assume as_self points to the furst entry in the free list
    _next = as_self;
    as_char += elementSize;

    // initialize the free list. Each element points to the next element.
    Freelist* runner = _next;
    for(size_t i = 1; i < numElements; ++i)
    {
        runner->_next = as_self;
        runner = as_self;
        as_char += elementSize;
    }

    runner->_next = nullptr;
}
