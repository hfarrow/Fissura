#include "stdafx.h"
#include <core/proxyallocator.h>
#include <core/assert.h>

using namespace fs;

ProxyAllocator::ProxyAllocator(const fschar* const  pName, Allocator& allocator, size_t budget)
	:
	_allocator(allocator),
	_totalUsedMemory(0),
	_totalNumAllocations(0),
	_budget(budget),
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

	FS_ASSERT_MSG_FORMATTED(_budget == 0 || _totalUsedMemory <= _budget, "ProxyAllocator named '%s' exceded memory budget.", getName());

	return pAllocation;
}

bool ProxyAllocator::deallocate(void* p)
{
	size_t oldTotalMemory = _allocator.getTotalUsedMemory();
	if(!_allocator.deallocate(p))
		return false;
	_totalUsedMemory -= oldTotalMemory - _allocator.getTotalUsedMemory();
	_totalNumAllocations -= 1;
	return true;
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
	_totalNumAllocations = 0;
	_totalUsedMemory = 0;
}