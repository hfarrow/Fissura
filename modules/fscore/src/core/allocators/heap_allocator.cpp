#include "fscore/memory/malloc.h"
#include "fscore/allocators/heap_allocator.h"
#include "fscore/allocators/page_allocator.h"
#include "fscore/utils/utils.h"
#include "fscore/utils/globals.h"
#include "fscore/debugging/assert.h"
#include "fscore/memory/new.h"

using namespace fs;

extern Allocator* gpVirtualAllocator;

HeapAllocator::HeapAllocator(const fschar* const  pName, size_t memorySize, void* pMemory)
	:
	Allocator(pName),
	_pMemory(pMemory),
	_memorySize(memorySize),
	_totalNumAllocations(0),
	_pBackingAllocator(nullptr)
{
	createHeap();	
}

HeapAllocator::HeapAllocator(const fschar* const pName, Allocator& backingAllocator)
	:
	Allocator(pName),
	_pMemory(nullptr),
	_memorySize(0),
	_totalNumAllocations(0),
	_pBackingAllocator(&backingAllocator)
{
	createHeap();
}

HeapAllocator::~HeapAllocator()
{
	FS_ASSERT(_totalNumAllocations == 0);
    FS_ASSERT(!gpVirtualAllocator);
    gpVirtualAllocator = _pBackingAllocator;
	destroy_mspace(_mspace);
    gpVirtualAllocator = nullptr;

}

void HeapAllocator::createHeap()
{
    FS_ASSERT(!gpVirtualAllocator);
    gpVirtualAllocator = _pBackingAllocator;
	if(_pBackingAllocator == nullptr)
	{
		_mspace = create_mspace_with_base(_pMemory, _memorySize, 0);
	}
	else
	{
		_mspace = create_mspace(0, 0);
	}
    gpVirtualAllocator = nullptr;

	FS_ASSERT_MSG(_mspace != nullptr, "Failled to create dlmalloc heap.");

}

void* HeapAllocator::allocate(size_t size, u8 alignment)
{
    FS_ASSERT(!gpVirtualAllocator);
    gpVirtualAllocator = _pBackingAllocator;
	void* p = mspace_memalign(_mspace, alignment, size);
	if(p == nullptr)
	{
		FS_ASSERT(!"Failed to allocate memory from dlmalloc heap.");
        gpVirtualAllocator = nullptr;
		return nullptr;
	}

#ifdef _DEBUG
	// TODO: replace this with mspace_footprint
	if(_memorySize > 0 && mspace_mallinfo(_mspace).uordblks > _memorySize)
	{
		FS_ASSERT(!"Heap exceded budget. Request will be served anyways if execution continues.");
	}
#endif

	_totalNumAllocations += 1;
    gpVirtualAllocator = nullptr;
	return p;
}

bool HeapAllocator::deallocate(void* p)
{
    FS_ASSERT(!gpVirtualAllocator);
    gpVirtualAllocator = _pBackingAllocator;
	mspace_free(_mspace, p);
	FS_ASSERT(_totalNumAllocations > 0);
	_totalNumAllocations -= 1;
    gpVirtualAllocator = nullptr;

	return true;
}

void HeapAllocator::clear()
{
	destroy_mspace(_mspace);
    _totalNumAllocations = 0;
	createHeap();
}

size_t HeapAllocator::getTotalUsedMemory() const
{
	return mspace_mallinfo(_mspace).uordblks;
}

u32 HeapAllocator::getTotalNumAllocations() const
{
	return _totalNumAllocations;
}
