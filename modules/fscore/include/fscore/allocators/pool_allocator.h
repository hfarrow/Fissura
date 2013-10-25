#ifndef FS_POOL_ALLOCATOR_H
#define FS_POOL_ALLOCATOR_H

#include "fscore/types.h"
#include "fscore/allocators/allocator.h"

namespace fs
{
	class PoolAllocator : public Allocator
	{
	public:
		PoolAllocator(const fschar* const  pName, size_t objectSize, u32 objAlignment, size_t memorySize, void* pMemory);
		~PoolAllocator();

		virtual void* allocate(size_t size, u8 alignment) override;
		virtual bool deallocate(void* p) override;
		virtual void clear() override;

		virtual bool canDeallocate() const override { return true; }
		virtual size_t getTotalUsedMemory() const override;
		virtual u32 getTotalNumAllocations() const override;

		void* allocateSingle();
		

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
