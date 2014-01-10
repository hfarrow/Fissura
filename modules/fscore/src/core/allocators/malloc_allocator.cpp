#include "fscore/allocators/malloc_allocator.h"

#include <stdlib.h>
#include <malloc.h>

#include "fscore/debugging/assert.h"

using namespace fs;

MallocAllocator::MallocAllocator(const fschar* pName) :
    Allocator(pName),
    _totalUsedMemory(0),
    _totalNumAllocations(0)
{
    
}

MallocAllocator::~MallocAllocator()
{

}

void* MallocAllocator::allocate(size_t size, u8 alignment)
{
    void* pAllocation = nullptr;
    i32 error = posix_memalign(&pAllocation, alignment, size);
    if(error != 0)
    {
        FS_ASSERT(!"MallocAllocator::allocate failed to allocate memory");
        return nullptr;
    }
    
    _totalUsedMemory += malloc_usable_size(pAllocation);
    _totalNumAllocations++;

    return pAllocation;
}

bool MallocAllocator::deallocate(void* p)
{
    if(!p)
    {
        return false;
    }

    _totalUsedMemory -= malloc_usable_size(p);
    _totalNumAllocations--;
    free(p);
    return true;
}

void MallocAllocator::clear()
{   
    // MallocAllocator would need to store a list of all allocations if
    // this function is to be implemented.
    FS_ASSERT(!"MallocAllocator does not implement clear()");
}

// Does not take allignment into account
size_t MallocAllocator::getTotalUsedMemory() const
{
    return _totalUsedMemory;
}

u32 MallocAllocator::getTotalNumAllocations() const
{
    return _totalNumAllocations;
}
