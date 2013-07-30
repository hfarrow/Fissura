#include "stdafx.h"
#include <core/traceallocator.h>
#include <core/heapallocator.h>

using namespace fissura;

TraceAllocator::TraceAllocator(const fschar* const  pName, Allocator& allocator)
	:
	ProxyAllocator(pName, allocator)
{
#ifdef _DEBUG
	// gDebugHeap must have been provided by application.
	FS_ASSERT(gDebugHeap != nullptr);
#endif
}

TraceAllocator::~TraceAllocator()
{
	// Unregister all stack traces
}

void* TraceAllocator::allocate(size_t size, u8 alignment)
{
	void* pAllocation = ProxyAllocator::allocate(size, alignment);

#ifdef _DEBUG
	if(pAllocation != nullptr)
	{
		// TODO: record stack trace for pAllocation
	}
#endif

	return pAllocation;
}

void TraceAllocator::deallocate(void* p)
{
	ProxyAllocator::deallocate(p);

#ifdef _DEBUG
	// TODO: remove stack trace for p
#endif
}