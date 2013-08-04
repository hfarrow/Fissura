#ifndef FS_HEAP_ALLOCATOR_H
#define FS_HEAP_ALLOCATOR_H

#include <core/types.h>
#include <core/allocator.h>
#include <dlmalloc/malloc.h>

namespace fissura
{
	class PageAllocator;

	class HeapAllocator : public Allocator
	{
	public:
		// Using this ctor will provide memory for the allocator to use. If it tries to
		// allocate more memory than is provided, an assert will be thrown. The allocation
		// will still be served using mmap and the application can continue if needed.
		// If you want a heap that can grow without asserting use the ctor that requires
		// a backing PageAllocator.
		HeapAllocator(const fschar* const  pName, size_t memorySize, void* pMemory);

		// Using a backing page allocator will allow the heap to grow and be tracked
		// through the backing allocator.
		HeapAllocator(const fschar* const pName, PageAllocator& backingAllocator);
		~HeapAllocator();

		void* allocate(size_t size, u8 alignment);
		bool deallocate(void* p);
		size_t getTotalUsedMemory() const;
		u32 getTotalNumAllocations() const;
		bool canDeallocate() const { return true; }
		void clear();

	private:
		void createHeap();

		void* _pMemory;
		size_t _memorySize;
		mspace _mspace;
		u32 _totalNumAllocations;
		PageAllocator* _pBackingAllocator;
	};
}

#endif