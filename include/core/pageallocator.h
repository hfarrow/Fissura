#ifndef FS_PAGE_ALLOCATOR_H
#define FS_PAGE_ALLOCATOR_H

#include <core/types.h>
#include <core/allocator.h>

namespace fs
{
	class PageAllocator : public Allocator
	{
	public:
		PageAllocator(const fschar* const pName);
		~PageAllocator();

		virtual void* allocate(size_t size, u8 alignment) override;
		virtual bool deallocate(void* p) override;
		virtual void clear() override;
		virtual bool canDeallocate() const override { return true; }
		virtual size_t getTotalUsedMemory() const override;
		virtual u32 getTotalNumAllocations() const override;

		void* allocateWithFlags(size_t size, u32 allocationTypeFlag, u32 protecFlag);
		u32 calcRequiredPages(size_t size) const;
		size_t getPageSize() const;

	private:
		u32 _totalNumAllocations;
		size_t _totalUsedMemory;
		size_t _pageSize;
	};
}

#endif