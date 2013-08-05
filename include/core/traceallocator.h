#ifndef FS_TRACE_ALLOCATOR_H
#define FS_TRACE_ALLOCATOR_H

#include <core/types.h>
#include <core/allocator.h>
#include <core/proxyallocator.h>
#include <core/stlallocator.h>
#include <memory>

namespace fs
{
	class TraceAllocator : public ProxyAllocator
	{
	public:
		struct AllocationInfo
		{
			static const size_t maxStackFrames = 10;
			size_t offsets[maxStackFrames];
		};

		TraceAllocator(const fschar* const  pName, Allocator& allocator)
			: ProxyAllocator(pName, allocator){}
		~TraceAllocator(){}

	protected:
		virtual void getStackTrace(AllocationInfo* pInfo, void* pAllocation) = 0;
		virtual const char* getCaller(const AllocationInfo* const pInfo) const = 0;

		typedef FS_DECL_MAP(uptr, AllocationInfo) AllocationMap;
		typedef FS_DECL_UNIQUE_PTR(AllocationMap) AllocationMapPointer;
		AllocationMapPointer _pAllocationMap;
	};
}

#endif