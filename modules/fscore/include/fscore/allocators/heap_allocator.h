#ifndef FS_HEAP_ALLOCATOR_H
#define FS_HEAP_ALLOCATOR_H

#include "fscore/types.h"
#include "fscore/allocators/allocator.h"
#include "fscore/allocators/page_allocator.h"
#include "fscore/allocators/stl_allocator.h"
#include "fscore/dlmalloc/malloc.h"
#include <vector>

namespace fs
{
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

		virtual void* allocate(size_t size, u8 alignment) override;
		virtual bool deallocate(void* p) override;
		virtual void clear() override;
		virtual bool canDeallocate() const override { return true; }
		virtual size_t getTotalUsedMemory() const override;
		virtual u32 getTotalNumAllocations() const override;		

	private:
		void createHeap();

		void* _pMemory;
		size_t _memorySize;
		mspace _mspace;
		u32 _totalNumAllocations;
		PageAllocator* _pBackingAllocator;

        static void pushVirtualAllocator(PageAllocator* _pBackingAllocator);
        static void popVirtualAllocator(PageAllocator* _pBackingAllocator);
        typedef std::vector<PageAllocator*, StlAllocator<PageAllocator>> VirtualAllocatorStack;
        static StlAllocator<PageAllocator>* _pStlAllocator;
        static VirtualAllocatorStack* _pVirtualAllocatorStack;
        static u32 _instanceCount;
	};
}

#endif
