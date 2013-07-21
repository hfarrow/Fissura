#include "stdafx.h"

#include <core/doublestackallocator.h>

using namespace fissura;

DoubleStackAllocator::DoubleStackAllocator(u32 stackSize, void* pStack)
	:
	_upper(stackSize, pStack, false),
	_lower(stackSize, pStack, true)
{
	_upper._pOther = &_lower;
	_lower._pOther = &_upper;
}

DoubleStackAllocator::~DoubleStackAllocator()
{

}

void* DoubleStackAllocator::allocateUpper(u32 size, u8 alignment)
{
	return _upper.allocate(size, alignment);
}

void* DoubleStackAllocator::allocateLower(u32 size, u8 alignment)
{
	return _lower.allocate(size, alignment);
}

void DoubleStackAllocator::deallocateUpper(StackAllocator::Marker& marker)
{
	_upper.deallocateToMarker(marker);
}

void DoubleStackAllocator::deallocateLower(StackAllocator::Marker& marker)
{
	_lower.deallocateToMarker(marker);
}

StackAllocator& DoubleStackAllocator::getUpper()
{
	return _upper;
}

StackAllocator& DoubleStackAllocator::getLower()
{
	return _lower;
}

u32 DoubleStackAllocator::getTotalUsedMemory() const
{
	return _upper.getTotalUsedMemory() + _lower.getTotalUsedMemory();
}

u32 DoubleStackAllocator::getTotalNumAllocations() const
{
	return _upper.getTotalNumAllocations() + _lower.getTotalNumAllocations();
}

void DoubleStackAllocator::clear()
{
	_upper.clear();
	_lower.clear();
}
