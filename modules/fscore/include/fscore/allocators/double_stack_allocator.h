#ifndef FS_DOUBLE_STACK_ALLOCATOR_H
#define FS_DOUBLE_STACK_ALLOCATOR_H

#include "fscore/utils/types.h"
#include "fscore/allocators/allocator.h"
#include "fscore/allocators/stack_allocator.h"

namespace fs
{
	class DoubleStackAllocator : public Allocator
	{
	public:
		DoubleStackAllocator(const fschar* const  pName, size_t stackSize, void* pStack);
		~DoubleStackAllocator();

		virtual void* allocate(size_t size, u8 alignment) override;
		virtual bool deallocate(void* p) override;
		virtual void clear() override;
		virtual bool canDeallocate() const override { return false; }
		virtual size_t getTotalUsedMemory() const override;
		virtual u32 getTotalNumAllocations() const override;

		void* allocateUpper(size_t size, u8 alignment);
		void* allocateLower(size_t size, u8 alignment);
		void deallocateUpper(StackAllocator::Marker& marker);
		void deallocateLower(StackAllocator::Marker& marker);
		StackAllocator& getUpper();
		StackAllocator& getLower();

	private:
		StackAllocator _upper;
		StackAllocator _lower;
	};
}

#endif
