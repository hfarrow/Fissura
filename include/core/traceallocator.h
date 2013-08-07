#ifndef FS_TRACE_ALLOCATOR_H
#define FS_TRACE_ALLOCATOR_H

#include <core/platforms.h>
#include <core/types.h>
#include <core/trace.h>
#include <core/allocator.h>
#include <core/proxyallocator.h>
#include <core/stlallocator.h>
#include <memory>

/*
TraceAllocator is a platform specific class that extends ProxyAllocator.
If a platform version of the class is not provided in the platform lib, 
it will behave exactly like a ProxyAllocator.
 */
namespace fs
{
namespace internal
{
	template<int PlatformID>
	class TraceAllocator : public ProxyAllocator
	{
	public:
 		TraceAllocator(const fschar* const  pName, Allocator& allocator);
		~TraceAllocator();

		virtual void* allocate(size_t size, u8 alignment) override;
		virtual bool deallocate(void* p) override;
	};

	#include <core/traceallocator.inl>
}
	typedef internal::TraceAllocator<PLATFORM_ID> TraceAllocator;
}

// Include platform specific headers if they exist
#include <windows/traceallocator.h>

#endif