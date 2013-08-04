#ifndef FS_PAGE_ALLOCATOR_H
#define FS_PAGE_ALLOCATOR_H

#include <core/types.h>
#include <core/allocator.h>

namespace fissura
{
	class PageAllocator : public Allocator
	{
	public:
		PageAllocator(const fschar* const pName);
		~PageAllocator();

		void* allocate(size_t size, u8 alignment);
		void deallocate(void* p);
		size_t getTotalUsedMemory() const;
		u32 getTotalNumAllocations() const;
		bool canDeallocate() const { return true; }
		void clear();

		u32 calcRequiredPages(size_t size) const;
		size_t getPageSize() const;

	private:
		u32 _totalNumAllocations;
		size_t _totalUsedMemory;
		size_t _pageSize;
	};
}

#endif