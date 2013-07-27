#ifndef FS_DOUBLE_BUFFERED_ALLOCATOR_H
#define FS_DOUBLE_BUFFERED_ALLOCATOR_H

#include <core/allocator.h>
#include <core/stackallocator.h>

namespace fissura
{
	class DoubleBufferedAllocator : public Allocator
	{
	public:
		DoubleBufferedAllocator(StackAllocator& stackA, StackAllocator& stackB);
		~DoubleBufferedAllocator();

		void* allocate(size_t size, u8 alignment);
		void deallocate(void* p);

		size_t getTotalUsedMemory() const;
		u32 getTotalNumAllocations() const;
		bool canDeallocate() const { return false; }

		void swapBuffers();
		void clearCurrentBuffer();
		void clearOtherBuffer();

		const StackAllocator& getStackA() const;
		const StackAllocator& getStackB() const;

	private:
		StackAllocator* _stacks[2];
		u32 _currentStack;
	};
}

#endif