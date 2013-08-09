#ifndef FS_WINDOWS_PAGE_ALLOCATOR_H
#define FS_WINDOWS_PAGE_ALLOCATOR_H

#include <memory>

#include "core/platforms.h"
#include "core/types.h"
#include "core/allocators/page_allocator.h"
#include "core/allocators/allocator.h"
#include "core/allocators/stl_allocator.h"

namespace fs
{
namespace internal
{
	template<>
	class PageAllocator<PLATFORM_WINDOWS> : public Allocator
	{
	public:
		PageAllocator(const fschar* const  pName);
		~PageAllocator();

		virtual void* allocate(size_t size, u8 alignment) override;
		virtual bool deallocate(void* p) override;
		virtual void clear() override;
		virtual bool canDeallocate() const override { return true; }
		virtual size_t getTotalUsedMemory() const override;
		virtual u32 getTotalNumAllocations() const override;

		u32 calcRequiredPages(size_t size) const;
		size_t getPageSize() const;

		// Windows only function. Compiler error if referenced from other platform.
		void* allocateWithFlags(size_t size, u32 allocationTypeFlag, u32 protecFlag);

	private:
		u32 _totalNumAllocations;
		size_t _totalUsedMemory;
		size_t _pageSize;
	};
}
}

#endif