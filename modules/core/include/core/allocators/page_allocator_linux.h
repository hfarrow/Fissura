#ifndef FS_PAGE_ALLOCATOR_LINUX_H
#define FS_PAGE_ALLOCATOR_LINUX_H

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
	class PageAllocator<PLATFORM_LINUX> : public Allocator
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

	private:
		u32 _totalNumAllocations;
		size_t _totalUsedMemory;
		size_t _pageSize;
	};
}
}

#endif
