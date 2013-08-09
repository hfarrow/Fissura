#include "stdafx.h"
#include "core/dlmalloc/malloc.h"
#include "core/allocators/heap_allocator.h"
#include "core/allocators/page_allocator.h"
#include "core/util.h"
#include "core/assert.h"

using namespace fs;

extern PageAllocator* gpVirtualAllocator;

HeapAllocator::HeapAllocator(const fschar* const  pName, size_t memorySize, void* pMemory)
	:
	_pMemory(pMemory),
	_memorySize(memorySize),
	_totalNumAllocations(0),
	_pBackingAllocator(nullptr),
	Allocator(pName)
{
	createHeap();	
}

HeapAllocator::HeapAllocator(const fschar* const pName, PageAllocator& backingAllocator)
	:
	_pMemory(nullptr),
	_memorySize(0),
	_totalNumAllocations(0),
	_pBackingAllocator(&backingAllocator),
	Allocator(pName)
{
	createHeap();
}

HeapAllocator::~HeapAllocator()
{
	FS_ASSERT(_totalNumAllocations == 0);
	gpVirtualAllocator = _pBackingAllocator;
	destroy_mspace(_mspace);
	gpVirtualAllocator = nullptr;
}

void HeapAllocator::createHeap()
{
	gpVirtualAllocator = _pBackingAllocator;
	if(_pBackingAllocator == nullptr)
	{
		_mspace = create_mspace_with_base(_pMemory, _memorySize, 0);
	}
	else
	{
		_mspace = create_mspace(0, 0);
	}

	FS_ASSERT_MSG(_mspace != nullptr, "Failled to create dlmalloc heap.");

	gpVirtualAllocator = nullptr;
}

void* HeapAllocator::allocate(size_t size, u8 alignment)
{
	gpVirtualAllocator = _pBackingAllocator;
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
	gpVirtualAllocator = nullptr;
	return p;
}

bool HeapAllocator::deallocate(void* p)
{
	gpVirtualAllocator = _pBackingAllocator;
	mspace_free(_mspace, p);
	FS_ASSERT(_totalNumAllocations > 0);
	_totalNumAllocations -= 1;
	gpVirtualAllocator = nullptr;

	return true;
}

void HeapAllocator::clear()
{
	gpVirtualAllocator = _pBackingAllocator;
	destroy_mspace(_mspace);
	createHeap();
	gpVirtualAllocator = nullptr;
}

size_t HeapAllocator::getTotalUsedMemory() const
{
	return mspace_mallinfo(_mspace).uordblks;
}

u32 HeapAllocator::getTotalNumAllocations() const
{
	return _totalNumAllocations;
}
