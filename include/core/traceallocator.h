#ifndef FS_TRACE_ALLOCATOR_H
#define FS_TRACE_ALLOCATOR_H

#include <core/types.h>
#include <core/allocator.h>
#include <core/proxyallocator.h>

namespace fissura
{
	class TraceAllocator : public ProxyAllocator
	{
	public:
		struct AllocationInfo
		{
			static const size_t maxStackFrames = 10;
			size_t offsets[maxStackFrames];
		};

		TraceAllocator(const fschar* const  pName, Allocator& allocator);
		~TraceAllocator();

		void* allocate(size_t size, u8 alignment);
		void deallocate(void* p);

	private:
		// void saveStackTrace(AllocationInfo* pInfo);
		// coid unsaveStackTrace(AllocationInfo* pInfo);
		// const char* getCallers(const AllocationInfo* const pInfo);
	};
}

#endif