#include "fscore/memory/heap_allocator.h"

#include "fscore/memory/utils.h"

using namespace fs;

namespace
{
    static const size_t SIZE_OF_ALLOCATION_OFFSET = sizeof(fs::u32);
    static_assert(SIZE_OF_ALLOCATION_OFFSET == 4, "Allocation offset has wrong size.");
}

HeapAllocator::HeapAllocator(void* start, void* end) :
    _start(start),
    _end(end)
{
    FS_ASSERT(start);
    FS_ASSERT(end);
    FS_ASSERT(start < end);
    createHeap();
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
    // store the allocation offset infront of the allocation
    size += SIZE_OF_ALLOCATION_OFFSET;
    offset += SIZE_OF_ALLOCATION_OFFSET;

    // We waste up to 'alignment' bytes in order to ensure we can align and 
    // offset the memory as requested.
    uptr ptr = (uptr)mspace_malloc(_mspace, size + alignment);

    if((void*)ptr == nullptr)
    {
        FS_ASSERT(!"Failed to allocate memory from dlmalloc heap.");
        return nullptr;
    }
    
    void* header = (void*)(pointerUtil::alignTop(ptr + offset, alignment) - offset);
    const u32 headerSize = (uptr)header - ptr + SIZE_OF_ALLOCATION_OFFSET;

    union
    {
        void* as_void;
        char* as_char;
        u32* as_u32;
        uptr as_uptr;
    };

    as_void = header;
    *as_u32 = headerSize;
    as_char += SIZE_OF_ALLOCATION_OFFSET;
    void* userPtr = as_void;

    return userPtr;
}

void HeapAllocator::free(void* ptr)
{
    FS_ASSERT(ptr);
    FS_ASSERT(ptr >= _start && ptr < _end);

    union
    {
        void* as_void;
        char* as_char;
        u32* as_u32;
        uptr as_uptr;
    };

    as_void = ptr;
    as_char -= SIZE_OF_ALLOCATION_OFFSET;
    const u32 headerSize = *as_u32;
    FS_ASSERT(headerSize >= SIZE_OF_ALLOCATION_OFFSET);
    
    as_uptr = (uptr)ptr - headerSize; 
    mspace_free(_mspace, as_void);
}

void HeapAllocator::reset()
{
    destroy_mspace(_mspace);
    createHeap();
}

size_t HeapAllocator::getTotalUsedSize()
{
    return mspace_mallinfo(_mspace).uordblks;
}
