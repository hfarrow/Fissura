#include "fscore/allocators/thread_safe_allocator.h"

#include <boost/thread.hpp>

#include "fscore/debugging/assert.h"

using namespace fs;

ThreadSafeAllocator::ThreadSafeAllocator(const fschar* const  pName, Allocator& allocator) :
    Allocator(pName),
    _allocator(allocator),
    _totalUsedMemory(0),
    _totalNumAllocations(0)
{

}

ThreadSafeAllocator::~ThreadSafeAllocator()
{

}

void* ThreadSafeAllocator::allocate(size_t size, u8 alignment)
{
    boost::lock_guard<boost::mutex> lock(_mutex);
	void* pAllocation = _allocator.allocate(size, alignment);
	if(pAllocation == nullptr)
	{
		return nullptr;
	}

    _totalUsedMemory = _allocator.getTotalUsedMemory();
    _totalNumAllocations = _allocator.getTotalNumAllocations();

	return pAllocation;
}

bool ThreadSafeAllocator::deallocate(void* p)
{
    boost::lock_guard<boost::mutex> lock(_mutex);
	if(!_allocator.deallocate(p))
		return false;

    _totalUsedMemory = _allocator.getTotalUsedMemory();
    _totalNumAllocations = _allocator.getTotalNumAllocations();

	return true;
}

bool ThreadSafeAllocator::deallocate(void* p, size_t size)
{
    boost::lock_guard<boost::mutex> lock(_mutex);
	if(!_allocator.deallocate(p, size))
		return false;

    _totalUsedMemory = _allocator.getTotalUsedMemory();
    _totalNumAllocations = _allocator.getTotalNumAllocations();

	return true;
}

size_t ThreadSafeAllocator::getTotalUsedMemory() const
{
    return _totalUsedMemory;
}

u32 ThreadSafeAllocator::getTotalNumAllocations() const
{
    return _totalNumAllocations;
}

bool ThreadSafeAllocator::canDeallocate() const
{
	return _allocator.canDeallocate();
}

void ThreadSafeAllocator::clear()
{
    boost::lock_guard<boost::mutex> lock(_mutex);
	_allocator.clear();
}
