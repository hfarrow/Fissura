// Default implementation if not platform specialization is provided.

template<int PlatformID>
TraceAllocator<PlatformID>::TraceAllocator(const fschar* const  pName, Allocator& allocator)
	: ProxyAllocator(pName, allocator)
{
	FS_TRACE("DEFAULT TraceAllocator ctor");
}

template<int PlatformID>
TraceAllocator<PlatformID>::~TraceAllocator()
{
	FS_TRACE("DEFAULT TraceAllocator ~ctor");
}

template<int PlatformID>
void* TraceAllocator<PlatformID>::allocate(size_t size, u8 alignment)
{
	FS_TRACE("DEFAULT TraceAllocator allocate");
	return ProxyAllocator::allocate(size, alignment);
}

template<int PlatformID>
bool TraceAllocator<PlatformID>::deallocate(void* p)
{
	FS_TRACE("DEFAULT TraceAllocator deallocate");
	return ProxyAllocator::deallocate(p);
}