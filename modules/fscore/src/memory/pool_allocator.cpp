#include "fscore/memory/pool_allocator.h"

#include "fscore/memory/utils.h"
#include "fscore/debugging/assert.h"

using namespace fs;

PoolAllocator::PoolAllocator(void* start, void* end, size_t maxElementSize, size_t maxAlignment, size_t offset) :
    _maxElementSize(maxElementSize),
    _maxAlignment(maxAlignment),
    _offset(offset),
    _start(start),
    _end(end),
    _freelist(start, end, maxElementSize, maxAlignment, _offset)
{

}

PoolAllocator::~PoolAllocator()
{
    if(_deleter)
    {
        _deleter();
    }
}

void* PoolAllocator::allocate(size_t size, size_t alignment, size_t offset)
{
    FS_ASSERT(size <= _maxElementSize);
    FS_ASSERT(alignment <= _maxAlignment);
    FS_ASSERT(offset == 0);

    void* userPtr = _freelist.obtain();

    if(!userPtr)
    {
        FS_ASSERT(!"PoolAllocator out of memory");
        return nullptr;
    }

    return userPtr;
}
