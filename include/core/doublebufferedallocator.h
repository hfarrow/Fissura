#ifndef FS_DOUBLE_BUFFERED_ALLOCATOR_H
#define FS_DOUBLE_BUFFERED_ALLOCATOR_H

#include <core/allocator.h>
#include <core/stackallocator.h>

namespace fissura
{
	class DoubleBufferedAllocator
	{
	public:
		DoubleBufferedAllocator(StackAllocator& stackA, StackAllocator& stackB);
		~DoubleBufferedAllocator();

		void* allocate(u32 size, u8 alignment);
		void deallocate(void* p);

		void swapBuffers();
		void clearCurrentBuffer();
		void clearOtherBuffer();

		const StackAllocator& getStackA() const;
		const StackAllocator& getStackB() const;

	private:
		DoubleBufferedAllocator(DoubleBufferedAllocator& other);
		DoubleBufferedAllocator& operator=( const DoubleBufferedAllocator& rhs );

		StackAllocator* _stacks[2];
		u32 _currentStack;
	};
}

#endif