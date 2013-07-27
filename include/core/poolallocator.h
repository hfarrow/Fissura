#ifndef FS_POOL_ALLOCATOR_H
#define FS_POOL_ALLOCATOR_H

#include <core/types.h>
#include <core/allocator.h>

namespace fissura
{
	class PoolAllocator : public Allocator
	{
	public:
		PoolAllocator(size_t objectSize, u32 objAlignment, size_t memorySize, void* pMemory);
		~PoolAllocator();

		void* allocateSingle();
		void* allocate(size_t size, u8 alignment);
		void deallocate(void* p);
		void clear();

		u32 getTotalUsedMemory() const;
		u32 getTotalNumAllocations() const;
		bool canDeallocate() const { return true; }

	private:
		void resetFreeList();

		size_t _objectSize;
		u32 _objectAlignment;
		size_t _memorySize;
		void* _pFreeList;
		void* _pMemory;
		u32 _maxObjects;
		u32 _totalNumAllocations;
	};
}

#endif
