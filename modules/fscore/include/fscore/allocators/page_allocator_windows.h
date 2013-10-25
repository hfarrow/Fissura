#ifndef FS_PAGE_ALLOCATOR_WINDOWS_H
#define FS_PAGE_ALLOCATOR_WINDOWS_H

#include <memory>

#include "fscore/platforms.h"
#include "fscore/types.h"
#include "fscore/allocators/page_allocator.h"
#include "fscore/allocators/allocator.h"
#include "fscore/allocators/stl_allocator.h"

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

        virtual bool deallocate(void* p, size_t size);

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
