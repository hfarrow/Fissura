#include "core/allocators/double_stack_allocator.h"
#include "core/assert.h"

using namespace fs;

DoubleStackAllocator::DoubleStackAllocator(const fschar* const  pName, size_t stackSize, void* pStack)
	:
	Allocator(pName),
	_upper(pName, stackSize, pStack, false),
	_lower(pName, stackSize, pStack, true)
{
	_upper._pOther = &_lower;
	_lower._pOther = &_upper;
}

DoubleStackAllocator::~DoubleStackAllocator()
{
	FS_ASSERT(_upper.getTotalNumAllocations() == 0);
	FS_ASSERT(_upper.getTotalUsedMemory() == 0);
	FS_ASSERT(_lower.getTotalNumAllocations() == 0);
	FS_ASSERT(_lower.getTotalUsedMemory() == 0);
}

void* DoubleStackAllocator::allocate(size_t size, u8 alignment)
{
	return _lower.allocate(size, alignment);
}

bool DoubleStackAllocator::deallocate(void* p)
{
	FS_ASSERT(!"Cannot deallocate from DoubleStackAllocator. Use deallocateLower or deallocateUpper instead.");
	return false;
}

void* DoubleStackAllocator::allocateUpper(size_t size, u8 alignment)
{
	return _upper.allocate(size, alignment);
}

void* DoubleStackAllocator::allocateLower(size_t size, u8 alignment)
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

size_t DoubleStackAllocator::getTotalUsedMemory() const
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
