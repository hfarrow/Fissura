#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>

#include "fscore/allocators/page_allocator_linux.h"
#include "fscore/platforms.h"
#include "fscore/allocators/heap_allocator.h"
#include "fscore/globals.h"
#include "fscore/assert.h"
#include "fscore/trace.h"

using namespace fs;

PageAllocator::PageAllocator(const fschar* const pName)
	:
	Allocator(pName),
	_totalNumAllocations(0),
	_totalUsedMemory(0),
    _allocationMapAllocator(*gpFsMainHeap)
{
	_pageSize = sysconf(_SC_PAGE_SIZE);

	// gpFsMainHeap must have been provided by application.
	FS_ASSERT(gpFsMainHeap != nullptr);

	 _pAllocationMap = UniquePtr<AllocationMap>(FS_NEW(AllocationMap)
            (std::less<uptr>(), _allocationMapAllocator),
            [](AllocationMap* p){FS_DELETE(p);});
}

PageAllocator::~PageAllocator()
{
	FS_ASSERT(_totalNumAllocations == 0);
	FS_ASSERT(_totalUsedMemory == 0);
    _pAllocationMap->clear();
}

void* PageAllocator::allocate(size_t size, u8 alignment)
{
	FS_ASSERT_MSG(alignment == 0, "PageAllocator is aligned by default. Please pass 0 for alignment.");

    void* pAllocation = mmap(0, size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    if(pAllocation == MAP_FAILED)
    {
        FS_ASSERT_MSG(false, "Failed to mmap request size. pAllocation == MAP_FAILED");
        return nullptr;
    }

	u32 numPagesRequired = calcRequiredPages(size);
	_totalUsedMemory += (size_t)numPagesRequired * _pageSize;
    _totalNumAllocations += 1;

    _pAllocationMap->insert(std::make_pair((uptr)pAllocation, size));

    return pAllocation;
}

bool PageAllocator::deallocate(void* p, size_t size)
{
	FS_ASSERT(p != nullptr);
	FS_ASSERT(size > 0);

    if(munmap(p, size))
    {
        FS_ASSERT_MSG_FORMATTED(false, "Failed to munmap pointer %p of size %u. errno: %i", p, size, errno);
        return true; // error
    }

    _totalNumAllocations -= 1;
    _totalUsedMemory -= size;

    if(_pAllocationMap->find((uptr)p) == _pAllocationMap->end())
    {
        FS_ASSERT(!"Failed to remove allocation from AllocationMap. Was it allocated by this allocator?");
    }
    else
    {
        _pAllocationMap->erase((uptr)p);
    }

    return false;
}

bool PageAllocator::deallocate(void* p)
{
    if(_pAllocationMap->find((uptr)p) == _pAllocationMap->end())
    {
        FS_ASSERT("Failed to deallocate memory. Size not found. Was it allocated by this allocator?");
        return true; // error
    }
    else
    {
       size_t size = _pAllocationMap->at((uptr)p);
       return deallocate(p, size);
    }
}

void PageAllocator::clear()
{
	if(_pAllocationMap->size() > 0)
	{
        AllocationMap copy = *_pAllocationMap;

		for(auto it = copy.begin(); it != copy.end(); ++it)
		{
            void* pAllocation = (void*)it->first;
            size_t allocationSize = it->second;
            deallocate(pAllocation, allocationSize);
		}
        
        _pAllocationMap->clear();
	}
}

size_t PageAllocator::getTotalUsedMemory() const
{
	return _totalUsedMemory;
}

u32 PageAllocator::getTotalNumAllocations() const
{
	return _totalNumAllocations;
}

u32 PageAllocator::calcRequiredPages(size_t size) const
{
	u32 numPages = (u32)(size / _pageSize);
	if(size % _pageSize > 0)
	{
		// If the size is not a multiple of _pageSize,
		// an extra page will need to be allocated to support
		// the size.
		numPages += 1;
	}

	return numPages;
}

size_t PageAllocator::getPageSize() const
{
	return _pageSize;
}
