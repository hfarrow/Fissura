#include "fscore/memory/heap_allocator.h"

using namespace fs;

HeapAllocator::HeapAllocator(void* start, void* end) :
    _start(start),
    _end(end)
{
    FS_ASSERT(start);
    FS_ASSERT(end);
    FS_ASSERT(start < end);
}

HeapAllocator::~HeapAllocator()
{
    if(_deleter)
    {
        _deleter();
    }
}

void HeapAllocator::createHeap()
{
    _mspace = create_mspace_with_base(_start, (uptr)_end - (uptr)_start, 0);
    FS_ASSERT_MSG(_mspace, "Failed to create dlmalloc heap");
}

void* HeapAllocator::allocate(size_t size, size_t alignment, size_t offset)
{
    // TODO: allocate size and manually offset returned memory.
    // can probably just use mspace_malloc and align manaually
    void* userPtr = mspace_memalign(_mspace, alignment, size);
    if(userPtr == nullptr)
    {
        FS_ASSERT(!"Failed to allocate memory from dlmalloc heap.");
    }

    return userPtr;
}

void HeapAllocator::free(void* ptr)
{
    mspace_free(_mspace, ptr);
}

void HeapAllocator::reset()
{
    destroy_mspace(_mspace);
    createHeap();
}
