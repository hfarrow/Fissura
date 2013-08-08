#ifndef FS_WIN_TRACE_ALLOCATOR_H
#define FS_WIN_TRACE_ALLOCATOR_H

#include <core/platforms.h>
#include <core/types.h>
#include <core/allocators/proxyallocator.h>
#include <core/allocators/traceallocator.h>
#include <core/allocators/stlallocator.h>
#include <memory>

namespace fs
{
namespace internal
{
	template<>
	class TraceAllocator<PLATFORM_WINDOWS> : public ProxyAllocator
	{
	public:
		TraceAllocator(const fschar* const  pName, Allocator& allocator);
		~TraceAllocator();

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