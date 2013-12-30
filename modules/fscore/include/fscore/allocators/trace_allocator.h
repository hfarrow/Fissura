#ifndef FS_TRACE_ALLOCATOR_H
#define FS_TRACE_ALLOCATOR_H

#include <memory>

#include "fscore/utils/platforms.h"
#include "fscore/utils/types.h"
#include "fscore/allocators/allocator.h"
#include "fscore/allocators/proxy_allocator.h"
#include "fscore/allocators/stl_allocator.h"

/*
TraceAllocator is a platform specific class that extends ProxyAllocator.
If a platform version of the class is not provided in the platform lib, 
it will behave exactly like a ProxyAllocator.
 */
namespace fs
{
namespace internal
{
	template<u32 PlatformID>
	class TraceAllocator : public ProxyAllocator
	{
	public:
 		TraceAllocator(const fswchar* const  pName, Allocator& allocator);
		~TraceAllocator();

        virtual void reportMemoryLeaks();

		virtual void* allocate(size_t size, u8 alignment) override;
		virtual bool deallocate(void* p) override;
	};
}
	typedef internal::TraceAllocator<PLATFORM_ID> TraceAllocator;
}
#include "fscore/allocators/trace_allocator.inl"

#if PLATFORM_ID == PLATFORM_WINDOWS
    #include "fscore/allocators/trace_allocator_windows.h"
#elif PLATFORM_ID == PLATFORM_LINUX
    #include "fscore/allocators/trace_allocator_linux.h"
#endif

#endif
