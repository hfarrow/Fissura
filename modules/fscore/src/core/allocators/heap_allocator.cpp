#include "fscore/dlmalloc/malloc.h"
#include "fscore/allocators/heap_allocator.h"
#include "fscore/allocators/page_allocator.h"
#include "fscore/util.h"
#include "fscore/globals.h"
#include "fscore/assert.h"
#include "fscore/trace.h"

using namespace fs;

extern PageAllocator* gpVirtualAllocator;
u32 HeapAllocator::_instanceCount = 0;
HeapAllocator::VirtualAllocatorStack* HeapAllocator::_pVirtualAllocatorStack = nullptr;
StlAllocator<PageAllocator>* HeapAllocator::_pStlAllocator = nullptr;

HeapAllocator::HeapAllocator(const fschar* const  pName, size_t memorySize, void* pMemory)
	:
	Allocator(pName),
	_pMemory(pMemory),
	_memorySize(memorySize),
	_totalNumAllocations(0),
	_pBackingAllocator(nullptr)
{
    HeapAllocator::_instanceCount++;
	createHeap();	
}

HeapAllocator::HeapAllocator(const fschar* const pName, PageAllocator& backingAllocator)
	:
	Allocator(pName),
	_pMemory(nullptr),
	_memorySize(0),
	_totalNumAllocations(0),
	_pBackingAllocator(&backingAllocator)
{
    HeapAllocator::_instanceCount++;
	createHeap();
}

HeapAllocator::~HeapAllocator()
{
    if(--HeapAllocator::_instanceCount == 0)
    {
        FS_DELETE(HeapAllocator::_pVirtualAllocatorStack);
        FS_DELETE(HeapAllocator::_pStlAllocator);
        HeapAllocator::_pVirtualAllocatorStack = nullptr;
        HeapAllocator::_pStlAllocator = nullptr;
    }

	FS_ASSERT(_totalNumAllocations == 0);
    // pushVirtualAllocator(_pBackingAllocator);
    gpVirtualAllocator = _pBackingAllocator;
	destroy_mspace(_mspace);
    // popVirtualAllocator(_pBackingAllocator);
    gpVirtualAllocator = nullptr;

}

void HeapAllocator::createHeap()
{
    //pushVirtualAllocator(_pBackingAllocator);
    gpVirtualAllocator = _pBackingAllocator;
	if(_pBackingAllocator == nullptr)
	{
		_mspace = create_mspace_with_base(_pMemory, _memorySize, 0);
	}
	else
	{
		_mspace = create_mspace(0, 0);
	}
    //popVirtualAllocator(_pBackingAllocator);
    gpVirtualAllocator = nullptr;

	FS_ASSERT_MSG(_mspace != nullptr, "Failled to create dlmalloc heap.");

}

void* HeapAllocator::allocate(size_t size, u8 alignment)
{
    pushVirtualAllocator(_pBackingAllocator);
	void* p = mspace_memalign(_mspace, alignment, size);
	if(p == nullptr)
	{
		FS_ASSERT(!"Failed to allocate memory from dlmalloc heap.");
        popVirtualAllocator(_pBackingAllocator);
		return nullptr;
	}

#ifdef _DEBUG
	// TODO: replace this with mspace_footprint
	if(_memorySize > 0 && mspace_mallinfo(_mspace).uordblks > _memorySize)
	{
		FS_ASSERT(!"Heap exceded budget. Request will be served anyways if execution continues.");
	}
#endif

	_totalNumAllocations += 1;
    popVirtualAllocator(_pBackingAllocator);
	return p;
}

bool HeapAllocator::deallocate(void* p)
{
    pushVirtualAllocator(_pBackingAllocator);
	mspace_free(_mspace, p);
	FS_ASSERT(_totalNumAllocations > 0);
	_totalNumAllocations -= 1;
    popVirtualAllocator(_pBackingAllocator);

	return true;
}

void HeapAllocator::clear()
{
    pushVirtualAllocator(_pBackingAllocator);
	destroy_mspace(_mspace);
	createHeap();
}

size_t HeapAllocator::getTotalUsedMemory() const
{
	return mspace_mallinfo(_mspace).uordblks;
}

u32 HeapAllocator::getTotalNumAllocations() const
{
	return _totalNumAllocations;
}

void HeapAllocator::pushVirtualAllocator(PageAllocator* _pBackingAllocator)
{
    if(!_pBackingAllocator)
    {
        return;
    }

    if(!_pVirtualAllocatorStack)
    {
        _pStlAllocator = FS_NEW(StlAllocator<PageAllocator>)(*gpFsMainHeap);
        _pVirtualAllocatorStack = FS_NEW(VirtualAllocatorStack)(*_pStlAllocator);
    }
    _pVirtualAllocatorStack->push_back(_pBackingAllocator);
    gpVirtualAllocator = _pBackingAllocator;
}

void HeapAllocator::popVirtualAllocator(PageAllocator* _pBackingAllocator)
{
    if(!_pBackingAllocator)
    {
        return;
    }

    FS_ASSERT(_pVirtualAllocatorStack != nullptr);
    FS_ASSERT(_pVirtualAllocatorStack->size() > 0);
    _pVirtualAllocatorStack->pop_back();
    if(_pVirtualAllocatorStack->size() > 0)
    {
        gpVirtualAllocator = _pVirtualAllocatorStack->back();
    }
}
