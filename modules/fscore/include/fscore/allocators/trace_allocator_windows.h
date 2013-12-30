#ifndef FS_WIN_TRACE_ALLOCATOR_H
#define FS_WIN_TRACE_ALLOCATOR_H

#include <memory>

#include "fscore/utils/platforms.h"
#include "fscore/utils/types.h"
#include "fscore/allocators/proxy_allocator.h"
#include "fscore/allocators/trace_allocator.h"
#include "fscore/allocators/stl_allocator.h"

namespace fs
{
namespace internal
{
	template<>
	class TraceAllocator<PLATFORM_WINDOWS> : public ProxyAllocator
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
			size_t offsets[maxStackFrames];
		};

		void getStackTrace(AllocationInfo* pInfo, void* pAllocation);
		const char* getCaller(const AllocationInfo* const pInfo) const;

		typedef FS_DECL_MAP(uptr, AllocationInfo) AllocationMap;
		typedef FS_DECL_UNIQUE_PTR(AllocationMap) AllocationMapPointer;
		AllocationMapPointer _pAllocationMap;
	};
}
}

#endif
