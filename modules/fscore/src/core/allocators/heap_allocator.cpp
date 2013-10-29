#include "fscore/dlmalloc/malloc.h"
#include "fscore/allocators/heap_allocator.h"
#include "fscore/allocators/page_allocator.h"
#include "fscore/util.h"
#include "fscore/assert.h"

using namespace fs;

extern PageAllocator* gpVirtualAllocator;

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

HeapAllocator::HeapAllocator(const fschar* const pName, PageAllocator& backingAllocator)
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
	gpVirtualAllocator = _pBackingAllocator != nullptr ? _pBackingAllocator : gpVirtualAllocator;
	destroy_mspace(_mspace);
}

void HeapAllocator::createHeap()
{
	gpVirtualAllocator = _pBackingAllocator != nullptr ? _pBackingAllocator : gpVirtualAllocator;
	if(_pBackingAllocator == nullptr)
	{
		_mspace = create_mspace_with_base(_pMemory, _memorySize, 0);
	}
	else
	{
		_mspace = create_mspace(0, 0);
	}

	FS_ASSERT_MSG(_mspace != nullptr, "Failled to create dlmalloc heap.");

}

void* HeapAllocator::allocate(size_t size, u8 alignment)
{
	gpVirtualAllocator = _pBackingAllocator != nullptr ? _pBackingAllocator : gpVirtualAllocator;
	void* p = mspace_memalign(_mspace, alignment, size);
	if(p == nullptr)
	{
		FS_ASSERT(!"Failed to allocate memory from dlmalloc heap.");
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
	return p;
}

bool HeapAllocator::deallocate(void* p)
{
	gpVirtualAllocator = _pBackingAllocator != nullptr ? _pBackingAllocator : gpVirtualAllocator;
	mspace_free(_mspace, p);
	FS_ASSERT(_totalNumAllocations > 0);
	_totalNumAllocations -= 1;

	return true;
}

void HeapAllocator::clear()
{
	gpVirtualAllocator = _pBackingAllocator != nullptr ? _pBackingAllocator : gpVirtualAllocator;
	destroy_mspace(_mspace);
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
