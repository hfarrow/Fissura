#ifndef FS_DOUBLE_STACK_ALLOCATOR_H
#define FS_DOUBLE_STACK_ALLOCATOR_H

#include <core/types.h>
#include <core/allocator.h>
#include <core/stackallocator.h>

namespace fissura
{
	class DoubleStackAllocator : public Allocator
	{
	public:
		DoubleStackAllocator(const fschar* const  pName, size_t stackSize, void* pStack);
		~DoubleStackAllocator();

		void* allocate(size_t size, u8 alignment);
		void deallocate(void* p);
		void* allocateUpper(size_t size, u8 alignment);
		void* allocateLower(size_t size, u8 alignment);
		void deallocateUpper(StackAllocator::Marker& marker);
		void deallocateLower(StackAllocator::Marker& marker);
		StackAllocator& getUpper();
		StackAllocator& getLower();
		size_t getTotalUsedMemory() const;
		u32 getTotalNumAllocations() const;
		bool canDeallocate() const { return false; }
		void clear();

	private:
		StackAllocator _upper;
		StackAllocator _lower;
	};
}

#endif