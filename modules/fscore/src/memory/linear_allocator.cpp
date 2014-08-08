#include "fscore/memory/allocators/linear_allocator.h"

#include "fscore/memory/utils.h"
#include "fscore/memory/allocators/page_allocator.h"

using namespace fs;



LinearAllocator::LinearAllocator(void* start, void* end) :
    _deleter(nullptr)
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
    if(_deleter)
    {
        _deleter();
    }
}

void* LinearAllocator::allocate(size_t size, size_t alignment, size_t offset)
{
    FS_ASSERT(alignment != 0);

    _current = pointerUtil::alignTop(_current + offset, alignment) - offset;
    void* userPtr = (void*) _current;
    _current += size;

    FS_ASSERT(userPtr);

    if(_current >= _end)
    {
        FS_ASSERT(!"LinearAllocator out of memory");
        return nullptr;
    }

    return userPtr;
}
