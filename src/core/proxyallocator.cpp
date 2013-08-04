#include "stdafx.h"
#include <core/proxyallocator.h>
#include <core/assert.h>

using namespace fissura;

ProxyAllocator::ProxyAllocator(const fschar* const  pName, Allocator& allocator)
	:
	_allocator(allocator),
	_totalUsedMemory(0),
	_totalNumAllocations(0),
	Allocator(pName)
{
}

ProxyAllocator::~ProxyAllocator()
{
	FS_ASSERT(_totalNumAllocations == 0);
	FS_ASSERT(_totalUsedMemory == 0);
}

void* ProxyAllocator::allocate(size_t size, u8 alignment)
{
	size_t oldTotalMemory = _allocator.getTotalUsedMemory();
	void* pAllocation = _allocator.allocate(size, alignment);
	if(pAllocation == nullptr)
	{
		return nullptr;
	}

	_totalUsedMemory += _allocator.getTotalUsedMemory() - oldTotalMemory;
	_totalNumAllocations += 1;

	return pAllocation;
}

void ProxyAllocator::deallocate(void* p)
{
	size_t oldTotalMemory = _allocator.getTotalUsedMemory();
	_allocator.deallocate(p);
	_totalUsedMemory -= oldTotalMemory - _allocator.getTotalUsedMemory();
	_totalNumAllocations -= 1;
}

size_t ProxyAllocator::getTotalUsedMemory() const
{
	return _totalUsedMemory;
}

u32 ProxyAllocator::getTotalNumAllocations() const
{
	return _totalNumAllocations;
}

bool ProxyAllocator::canDeallocate() const
{
	return _allocator.canDeallocate();
}

void ProxyAllocator::clear()
{
	_allocator.clear();
}