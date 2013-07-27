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
			const size_t position;
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

		StackAllocator(size_t stackSize, void* pStack);
		StackAllocator(size_t stackSize, void* pStack, bool growUpwards);
		~StackAllocator();

		void* allocate(size_t size, u8 alignment);
		void deallocate(void* p);
		size_t getTotalUsedMemory() const;
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

		void init(size_t stackSize, void* pStack, bool growUpwards);
		void* allocateUpwards(size_t, u8 alignment);
		void* allocateDownwards(size_t, u8 alignment);

		size_t _stackSize;
		void* _pStack;
		void* _pCurrentPosition;
		size_t _totalUsedMemory;
		u32 _totalNumAllocations;
		bool _growUpwards;

		// DoubleStackAllocator will set _pOther when
		// constructed.
		StackAllocator* _pOther;
	};
}

#endif