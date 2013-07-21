#include "stdafx.h";

#include <core/types.h>
#include <core/util.h>
#include <core/poolallocator.h>

using namespace fissura;

PoolAllocator::PoolAllocator(u32 objectSize, u32 objectAlignment, u32 memorySize, void* pMemory)
	:
	_objectSize(objectSize),
	_objectAlignment(objectAlignment),
	_memorySize(memorySize),
	_pMemory(pMemory),
	_totalNumAllocations(0)
{
	FS_ASSERT_MSG(_objectSize >= 4, "Pool allocator can not allocate objects smaller than 4 bytes");
	FS_ASSERT(_objectAlignment >= 4);
	FS_ASSERT(_memorySize >= 4);
	FS_ASSERT(pMemory != nullptr);

	u8 adjustment = alignAdjustment(pMemory, _objectAlignment);
	_pFreeList = (void*)((uptr)pMemory + adjustment);

	FS_ASSERT((uptr)_pFreeList < (uptr)pMemory + _memorySize);

	_maxObjects = (_memorySize - adjustment) / objectSize;

	resetFreeList();
}

PoolAllocator::~PoolAllocator()
{

}

void PoolAllocator::resetFreeList()
{
	// reset head to start if memory block
	_pFreeList = _pMemory;

	for(u32 i = 0; i < _maxObjects - 1; ++i)
	{
		// create the "next" pointers up to but not inluding the last object.
		uptr* pObject = (uptr*)((uptr)_pFreeList + (i * _objectSize));
		*pObject = (uptr)_pFreeList + ((i + 1) * _objectSize);

	}
	// Set the final free list pointer to null.
	uptr* pLastObject = (uptr*)((uptr)_pFreeList + ((_maxObjects - 1) * _objectSize));
	*pLastObject = 0;
}

void* PoolAllocator::allocateSingle()
{
	return allocate(_objectSize, _objectAlignment);
}

void* PoolAllocator::allocate(u32 size, u8 alignment)
{
	FS_ASSERT(size == _objectSize);
	FS_ASSERT(alignment == _objectAlignment);

	if(_pFreeList == nullptr)
	{
		FS_ASSERT(!"PoolAllocator out of memory");
		return nullptr;
	}

	// return the current head of the free list.
	void* pAllocation = _pFreeList;

	// Move the current head to the next pointer of the returned allocation.
	_pFreeList = (void*)(*((uptr*)_pFreeList));

	_totalNumAllocations += 1;

	return pAllocation;
}

void PoolAllocator::deallocate(void* p)
{
	if(p < _pMemory || p > (void*)((uptr)_pMemory + _memorySize))
	{
		FS_ASSERT(!"object to deallocate does not belong to this pool.");
	}

	// Point p to the current head and replace the current head with p.
	*(uptr*)p = (uptr)_pFreeList;
	_pFreeList = p;

	_totalNumAllocations -= 1;
}

void PoolAllocator::clear()
{
	resetFreeList();
}

u32 PoolAllocator::getTotalUsedMemory() const
{
	return _totalNumAllocations * _objectSize;
}

u32 PoolAllocator::getTotalNumAllocations() const
{
	return _totalNumAllocations;
}
