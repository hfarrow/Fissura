#include "stdafx.h"
#include <dlmalloc/malloc.h>
#include <core/heapallocator.h>
#include <core/util.h>

using namespace fissura;

HeapAllocator::HeapAllocator(size_t memorySize, void* pMemory)
	:
	_memorySize(memorySize),
	_totalNumAllocations(0)
{
	_mspace = create_mspace_with_base(pMemory, memorySize, 0);
	FS_ASSERT_MSG(_mspace != nullptr, "Failled to create dlmalloc heap.");
}

HeapAllocator::~HeapAllocator()
{
	FS_ASSERT(_totalNumAllocations == 0);
}

void* HeapAllocator::allocate(size_t size, u8 alignment)
{
	void* p = mspace_memalign(_mspace, alignment, size);
	if(p == nullptr)
	{
		FS_ASSERT(!"Failed to allocate memory from dlmalloc heap.");
		return nullptr;
	}

#ifdef _DEBUG
	if(mspace_mallinfo(_mspace).uordblks > _memorySize)
	{
		FS_ASSERT(!"Heap exceded budget. Request will be served anyways if execution continues.");
	}
#endif

	_totalNumAllocations += 1;
	return p;
}

void HeapAllocator::deallocate(void* p)
{
	mspace_free(_mspace, p);
	FS_ASSERT(_totalNumAllocations > 0);
	_totalNumAllocations -= 1;
}

size_t HeapAllocator::getTotalUsedMemory() const
{
	return mspace_mallinfo(_mspace).uordblks;
}

u32 HeapAllocator::getTotalNumAllocations() const
{
	return _totalNumAllocations;
}
