#ifndef FS_STACK_ALLOCATOR_H
#define FS_STACK_ALLOCATOR_H

#include "core/types.h"
#include "core/allocators/allocator.h"

namespace fs
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
			const size_t position;
			const u32 allocationIndex;

			Marker& operator=( const Marker& rhs )
			{
				// Allow the marker to be copy assigned.
				const_cast<size_t&>(position) = rhs.position;
				const_cast<u32&>(allocationIndex) = rhs.allocationIndex;
				return *this;
			}
		private:
			friend StackAllocator;
			Marker(size_t position, u32 allocationIndex)
				: position(position), allocationIndex(allocationIndex) {}
		};

		StackAllocator(const fschar* const  pName, size_t stackSize, void* pStack);
		StackAllocator(const fschar* const  pName, size_t stackSize, void* pStack, bool growUpwards);
		~StackAllocator();

		virtual void* allocate(size_t size, u8 alignment) override;
		virtual bool deallocate(void* p) override;
		virtual void clear() override;
		virtual bool canDeallocate() const override { return false; }
		virtual size_t getTotalUsedMemory() const override;
		virtual u32 getTotalNumAllocations() const override;

		Marker getMarker() const;
		void deallocateToMarker(const Marker& marker);

	private:
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