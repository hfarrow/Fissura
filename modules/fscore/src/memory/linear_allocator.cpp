#include "fscore/memory/linear_allocator.h"

#include "fscore/memory/utils.h"
#include "fscore/memory/page_allocator.h"

using namespace fs;

LinearAllocator::LinearAllocator(size_t size)
{
    FS_ASSERT(size > 0);

    static PageAllocator allocator;
    _pBackingAllocator = &allocator;

    void* ptr = allocator.allocate(size);
    FS_ASSERT_MSG(ptr, "Failed to allocate pages for LinearAllocator");

    _start = (uptr)ptr;
    _end = _start + size;
    _current = _start;
}

LinearAllocator::LinearAllocator(void* start, void* end) :
    _pBackingAllocator(nullptr)
{
    FS_ASSERT(start);
    FS_ASSERT(end);
    FS_ASSERT(start < end);
    _start = (uptr)start;
    _end = (uptr)end;
    _current = _start;
}

LinearAllocator::~LinearAllocator()
{
    if(_pBackingAllocator)
    {
        _pBackingAllocator->free((void*)_start, (size_t)(_end - _start));
    }
}

void* LinearAllocator::allocate(size_t size, size_t alignment, size_t offset)
{
    _current = pointerUtil::alignTop(_current + offset, alignment) - offset;
    void* userPtr = (void*) _current;
    _current += size;

    if(_current >= _end)
    {
        FS_ASSERT(!"LinearAllocator out of memory");
        return nullptr;
    }

    return userPtr;
}
