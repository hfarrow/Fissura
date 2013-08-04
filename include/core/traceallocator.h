#ifndef FS_TRACE_ALLOCATOR_H
#define FS_TRACE_ALLOCATOR_H

#include <core/types.h>
#include <core/allocator.h>
#include <core/proxyallocator.h>
#include <core/stlallocator.h>
#include <memory>

namespace fissura
{
	class TraceAllocator : public ProxyAllocator
	{
	public:
		struct AllocationInfo
		{
			static const size_t maxStackFrames = 10;
			DWORD64 offsets[maxStackFrames];
		};

		TraceAllocator(const fschar* const  pName, Allocator& allocator);
		~TraceAllocator();

		void* allocate(size_t size, u8 alignment);
		void deallocate(void* p);

	private:
#if defined(_DEBUG) && defined(WIN32)
		void getStackTrace(AllocationInfo* pInfo, void* pAllocation);
		const char* getCaller(const AllocationInfo* const pInfo) const;

		typedef FS_DECL_MAP(uptr, AllocationInfo) AllocationMap;
		typedef FS_DECL_UNIQUE_PTR(AllocationMap) AllocationMapPointer;
		AllocationMapPointer _pAllocationMap;
#endif
	};
}

#endif