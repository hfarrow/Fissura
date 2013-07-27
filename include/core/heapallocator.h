#ifndef FS_HEAP_ALLOCATOR_H
#define FS_HEAP_ALLOCATOR_H

#include <core/types.h>
#include <core/allocator.h>
#include <dlmalloc/malloc.h>

namespace fissura
{
	class HeapAllocator : public Allocator
	{
	public:
		HeapAllocator(u32 memorySize, void* pMemory);
		~HeapAllocator();

		void* allocate(u32 size, u8 alignment);
		void deallocate(void* p);
		u32 getTotalUsedMemory() const;
		u32 getTotalNumAllocations() const;
		bool canDeallocate() const { return true; }

	private:
		u32 _memorySize;
		mspace _mspace;
		u32 _totalNumAllocations;
	};
}

#endif