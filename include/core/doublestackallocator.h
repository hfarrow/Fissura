#ifndef CORE_DOUBLE_STACK_ALLOCATOR_H
#define CORE_DOUBLE_STACK_ALLOCATOR_H

#include <core/types.h>
#include <core/allocator.h>
#include <core/stackallocator.h>

namespace fissura
{
	class DoubleStackAllocator : public Allocator
	{
	public:
		DoubleStackAllocator(u32 stackSize, void* pStack);
		~DoubleStackAllocator();

		void* allocate(u32 size, u8 alignment);
		void deallocate(void* p);
		void* allocateUpper(u32 size, u8 alignment);
		void* allocateLower(u32 size, u8 alignment);
		void deallocateUpper(StackAllocator::Marker& marker);
		void deallocateLower(StackAllocator::Marker& marker);
		StackAllocator& getUpper();
		StackAllocator& getLower();
		u32 getTotalUsedMemory() const;
		u32 getTotalNumAllocations() const;
		void clear();

	private:
		StackAllocator _upper;
		StackAllocator _lower;
	};
}

#endif