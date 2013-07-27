#ifndef CORE_POOL_ALLOCATOR_H
#define CORE_POOL_ALLOCATOR_H

#include <core/types.h>
#include <core/allocator.h>

namespace fissura
{
	class PoolAllocator : public Allocator
	{
	public:
		PoolAllocator(u32 objectSize, u32 objAlignment, u32 memorySize, void* pMemory);
		~PoolAllocator();

		void* allocateSingle();
		void* allocate(u32 size, u8 alignment);
		void deallocate(void* p);
		void clear();

		u32 getTotalUsedMemory() const;
		u32 getTotalNumAllocations() const;
		bool canDeallocate() const { return true; }

	private:
		void resetFreeList();

		u32 _objectSize;
		u32 _objectAlignment;
		u32 _memorySize;
		void* _pFreeList;
		void* _pMemory;
		u32 _maxObjects;
		u32 _totalNumAllocations;
	};
}

#endif
