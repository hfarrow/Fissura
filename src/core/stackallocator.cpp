#include "stdafx.h"
#include <core/stackallocator.h>
#include <core/util.h>

using namespace fissura;

StackAllocator::StackAllocator(u32 stackSize, void* pStack)
{
	init(stackSize, pStack, true);
}

StackAllocator::StackAllocator(u32 stackSize, void* pStack, bool growUpwards)
{
	init(stackSize, pStack, growUpwards);
}

void StackAllocator::init(u32 stackSize, void* pStack, bool growUpwards)
{
	FS_ASSERT(stackSize > 0);
	FS_ASSERT(pStack);
	_pOther = nullptr;
	_stackSize = stackSize;
	_pStack = pStack;
	_growUpwards = growUpwards;
	_totalUsedMemory = 0;
	_totalNumAllocations = 0;

	if(growUpwards)
		_pCurrentPosition = pStack;
	else
		_pCurrentPosition = (void*)((uptr)pStack + stackSize);
}

StackAllocator::~StackAllocator()
{
	FS_ASSERT(_totalNumAllocations == 0);
	FS_ASSERT(_totalUsedMemory == 0);
}

void* StackAllocator::allocate(u32 size, u8 alignment)
{
	FS_ASSERT(size > 0);
	return _growUpwards ? allocateUpwards(size, alignment) : allocateDownwards(size, alignment);
}

void* StackAllocator::allocateUpwards(u32 size, u8 alignment)
{
	u8 adjustment = alignAdjustment(_pCurrentPosition, alignment);
	
	if((_pOther && _pOther->getTotalUsedMemory() + _totalUsedMemory + adjustment + size > _stackSize) ||
	   (_totalUsedMemory + adjustment + size > _stackSize))
	{
		// Not enough memory remains for this allocation.
		FS_ASSERT(!"Stack allocator out of memory");
		return nullptr;
	}

	uptr alignedAddress = (uptr) _pCurrentPosition + adjustment;

	_pCurrentPosition = (void*) (alignedAddress + size);
	_totalUsedMemory += adjustment + size;
	_totalNumAllocations += 1;

	return (void*) alignedAddress;
}

void* StackAllocator::allocateDownwards(u32 size, u8 alignment)
{
	u8 adjustment = alignAdjustment((void*)((uptr)_pCurrentPosition - (uptr)size), alignment);
	if(adjustment > 0)
		adjustment = alignment - adjustment;
	
	if((_pOther && _pOther->getTotalUsedMemory() + _totalUsedMemory + adjustment + size > _stackSize) ||
	   (_totalUsedMemory + adjustment + size > _stackSize))
	{
		// Not enough memory remains for this allocation.
		FS_ASSERT(!"Stack allocator out of memory");
		return nullptr;
	}
	uptr alignedAddress = (uptr) _pCurrentPosition - size - adjustment;

	_pCurrentPosition = (void*)alignedAddress;
	_totalUsedMemory += adjustment + size;
	_totalNumAllocations += 1;

	return (void*) alignedAddress;
}

void StackAllocator::deallocate(void* p)
{
	FS_ASSERT(!"Cannot deallocate from StackAllocator. Use deallocateToMarker instead.");
}

u32 StackAllocator::getTotalUsedMemory() const
{
	return _totalUsedMemory;
}

u32 StackAllocator::getTotalNumAllocations() const
{
	return _totalNumAllocations;
}

StackAllocator::Marker StackAllocator::getMarker() const
{
	return StackAllocator::Marker((u32)_pCurrentPosition, _totalNumAllocations);
}

void StackAllocator::deallocateToMarker(const StackAllocator::Marker& marker)
{
	if(_growUpwards)
	{
		FS_ASSERT((uptr)marker.position <= (uptr)_pCurrentPosition);
		FS_ASSERT((uptr)marker.position >= (uptr)_pStack);
		_totalUsedMemory = marker.position - (u32)_pStack;
	}
	else
	{
		FS_ASSERT((uptr)marker.position >= (uptr)_pCurrentPosition);
		FS_ASSERT((uptr)marker.position <= (uptr)_pStack + _stackSize);
		_totalUsedMemory = (uptr)_pStack + _stackSize - marker.position;
	}

	_totalNumAllocations = marker.allocationIndex;
	_pCurrentPosition = (void*)marker.position;
}

void StackAllocator::clear()
{
	StackAllocator::Marker* pMarker = nullptr;
	if(_growUpwards)
		pMarker = &StackAllocator::Marker((u32)_pStack, 0);
	else
		pMarker = &StackAllocator::Marker((u32)_pStack + _stackSize, 0);
	
	deallocateToMarker(*pMarker);
}