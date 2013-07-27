#ifndef FS_STACK_ALLOCATOR_H
#define FS_STACK_ALLOCATOR_H

#include <core/types.h>
#include <core/allocator.h>

namespace fissura
{
	class StackAllocator : public Allocator
	{
	public:
		friend class DoubleStackAllocator;

		// A stack marker represents the current top of the stack.
		// A stack can only be rolled back to an existing marker and
		// not to an arbirtrary location within the stack.
		struct Marker
		{
			friend StackAllocator;
			const u32 position;
			const u32 allocationIndex;

			Marker& operator=( const Marker& rhs )
			{
				// Allow the marker to be copy assigned.
				const_cast<u32&>(position) = rhs.position;
				const_cast<u32&>(allocationIndex) = rhs.allocationIndex;
				return *this;
			}
		private:
			Marker(u32 position, u32 allocationIndex)
				: position(position), allocationIndex(allocationIndex) {}
		};

		StackAllocator(u32 stackSize, void* pStack);
		StackAllocator(u32 stackSize, void* pStack, bool growUpwards);
		~StackAllocator();

		void* allocate(u32 size, u8 alignment);
		void deallocate(void* p);
		u32 getTotalUsedMemory() const;
		u32 getTotalNumAllocations() const;
		bool canDeallocate() const { return false; }

		Marker getMarker() const;
		void deallocateToMarker(const Marker& marker);
		void clear();

	private:
		/*
		// No header required for stack allocator.
		// Leaving struct here for easy addiditon of header
		// at later time.
		struct AllocationHeader
		{
			u8 adjustment;
		};
		*/

		void init(u32 stackSize, void* pStack, bool growUpwards);
		void* allocateUpwards(u32, u8 alignment);
		void* allocateDownwards(u32, u8 alignment);

		u32 _stackSize;
		void* _pStack;
		void* _pCurrentPosition;
		u32 _totalUsedMemory;
		u32 _totalNumAllocations;
		bool _growUpwards;

		// DoubleStackAllocator will set _pOther when
		// constructed.
		StackAllocator* _pOther;
	};
}

#endif