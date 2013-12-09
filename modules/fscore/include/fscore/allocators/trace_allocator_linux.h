#ifndef FS_WIN_TRACE_ALLOCATOR_H
#define FS_WIN_TRACE_ALLOCATOR_H

#include <memory>

#include "fscore/platforms.h"
#include "fscore/types.h"
#include "fscore/allocators/proxy_allocator.h"
#include "fscore/allocators/trace_allocator.h"
#include "fscore/allocators/stl_allocator.h"

namespace fs
{
namespace internal
{
	template<>
	class TraceAllocator<PLATFORM_LINUX> : public ProxyAllocator
	{
	public:
		TraceAllocator(const fswchar* const  pName, Allocator& allocator);
		~TraceAllocator();

        virtual void reportMemoryLeaks();

		virtual void* allocate(size_t size, u8 alignment) override;
		virtual bool deallocate(void* p) override;

	private:
		struct AllocationInfo
		{
			static const size_t maxStackFrames = 10;
			void* frames[maxStackFrames];
            size_t numFrames;
		};

		void getStackTrace(AllocationInfo* pInfo, void* pAllocation);
		const char* getCaller(const AllocationInfo* const pInfo) const;

		typedef Map<uptr, AllocationInfo> AllocationMap;
        typedef MapAllocator<uptr, AllocationInfo> AllocationMapAllocator;
		UniquePtr<AllocationMap> _pAllocationMap;
        AllocationMapAllocator _allocationMapAllocator;
	};
}
}

#endif
