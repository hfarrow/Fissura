#ifndef FS_TRACE_ALLOCATOR_H
#define FS_TRACE_ALLOCATOR_H

#include <memory>

#include "core/platforms.h"
#include "core/types.h"
#include "core/trace.h"
#include "core/allocators/allocator.h"
#include "core/allocators/proxy_allocator.h"
#include "core/allocators/stl_allocator.h"

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
 		TraceAllocator(const fschar* const  pName, Allocator& allocator);
		~TraceAllocator();

		virtual void* allocate(size_t size, u8 alignment) override;
		virtual bool deallocate(void* p) override;
	};
}
	typedef internal::TraceAllocator<PLATFORM_ID> TraceAllocator;
}
#include "core/allocators/trace_allocator.inl"

#ifdef WIN32
// Include platform specific headers if they exist
#include "windows/allocators/trace_allocator.h"
#endif

#endif
