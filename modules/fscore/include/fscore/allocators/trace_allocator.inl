#ifndef FS_TRACE_ALLOCATOR_INL
#define FS_TRACE_ALLOCATOR_INL

#include "core/types.h"
#include "core/allocators/trace_allocator.h"

namespace fs
{
namespace internal
{
// Default implementation if no platform specialization is provided.

template<u32 PlatformID>
TraceAllocator<PlatformID>::TraceAllocator(const fschar* const  pName, Allocator& allocator)
	: ProxyAllocator(pName, allocator)
{
	FS_TRACE("DEFAULT PLATFORM TraceAllocator ctor");
}

template<u32 PlatformID>
TraceAllocator<PlatformID>::~TraceAllocator()
{
	FS_TRACE("DEFAULT PLATFORM TraceAllocator ~ctor");
}

template<u32 PlatformID>
void TraceAllocator<PlatformID>::reportMemoryLeaks()
{
	FS_TRACE("DEFAULT PLATFORM TraceAllocator reportMemoryLeaks");
}

template<u32 PlatformID>
void* TraceAllocator<PlatformID>::allocate(size_t size, u8 alignment)
{
	FS_TRACE_WARN("DEFAULT PLATFORM TraceAllocator allocate");
	return ProxyAllocator::allocate(size, alignment);
}

template<u32 PlatformID>
bool TraceAllocator<PlatformID>::deallocate(void* p)
{
	FS_TRACE_WARN("DEFAULT PLATFORM TraceAllocator deallocate");
	return ProxyAllocator::deallocate(p);
}

} // fs
} // internal

#endif
