#ifndef FS_DOUBLE_BUFFERED_ALLOCATOR_H
#define FS_DOUBLE_BUFFERED_ALLOCATOR_H

#include "fscore/allocators/allocator.h"
#include "fscore/allocators/stack_allocator.h"

namespace fs
{
	class DoubleBufferedAllocator : public Allocator
	{
	public:
		DoubleBufferedAllocator(const fschar* const  pName, StackAllocator& stackA, StackAllocator& stackB);
		~DoubleBufferedAllocator();

		virtual void* allocate(size_t size, u8 alignment) override;
		virtual bool deallocate(void* p) override;
		virtual void clear() override;
		virtual bool canDeallocate() const override { return false; }
		virtual size_t getTotalUsedMemory() const override;
		virtual u32 getTotalNumAllocations() const override;

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
