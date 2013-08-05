#ifndef FS_WIN_TRACE_ALLOCATOR_H
#define FS_WIN_TRACE_ALLOCATOR_H

#include <Windows.h>
#include <core/types.h>
#include <core/allocator.h>
#include <core/proxyallocator.h>
#include <core/stlallocator.h>
#include <core/traceallocator.h>
#include <memory>

namespace fs
{
namespace windows
{
	class TraceAllocator : public fs::TraceAllocator
	{
	public:
		TraceAllocator(const fschar* const  pName, Allocator& allocator);
		~TraceAllocator();

		virtual void* allocate(size_t size, u8 alignment) override;
		virtual bool deallocate(void* p) override;

	protected:
		virtual void getStackTrace(AllocationInfo* pInfo, void* pAllocation) override;
		virtual const char* getCaller(const AllocationInfo* const pInfo) const override;
	};
}
}

#endif