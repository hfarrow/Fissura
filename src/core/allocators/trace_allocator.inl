#ifndef FS_TRACE_ALLOCATOR_INL
#define FS_TRACE_ALLOCATOR_INL

namespace fs
{
namespace internal
{
// Default implementation if not platform specialization is provided.

template<u32 PlatformID>
TraceAllocator<PlatformID>::TraceAllocator(const fschar* const  pName, Allocator& allocator)
	: ProxyAllocator(pName, allocator)
{
	FS_TRACE("DEFAULT TraceAllocator ctor");
}

template<u32 PlatformID>
TraceAllocator<PlatformID>::~TraceAllocator()
{
	FS_TRACE("DEFAULsT TraceAllocator ~ctor");s
}

template<u32 PlatformID>
void* TraceAllocator<PlatformID>::allocate(size_t size, u8 alignment)
{
	FS_TRACE("DEFAULT TraceAllocator allocate");
	return ProxyAllocator::allocate(size, alignment);
}

template<u32 PlatformID>
bool TraceAllocator<PlatformID>::deallocate(void* p)
{
	FS_TRACE("DEFAULT TraceAllocator deallocate");
	return ProxyAllocator::deallocate(p);
}

} // fs
} // internal

#endif