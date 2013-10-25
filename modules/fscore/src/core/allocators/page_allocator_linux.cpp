#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>

#include "fscore/allocators/page_allocator_linux.h"
#include "fscore/platforms.h"
#include "fscore/globals.h"
#include "fscore/assert.h"
#include "fscore/trace.h"

using namespace fs;

PageAllocator::PageAllocator(const fschar* const pName)
	:
	Allocator(pName),
	_totalNumAllocations(0),
	_totalUsedMemory(0)
{
	_pageSize = sysconf(_SC_PAGE_SIZE);
}

PageAllocator::~PageAllocator()
{
	FS_ASSERT(_totalNumAllocations == 0);
	FS_ASSERT(_totalUsedMemory == 0);
}

void* PageAllocator::allocate(size_t size, u8 alignment)
{
	FS_ASSERT_MSG(alignment == 0, "PageAllocator is aligned by default. Please pass 0 for alignment.");

    void* ptr = mmap(0, size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    FS_ASSERT_MSG(ptr != MAP_FAILED, "Failed to mmap request size.");

	u32 numPagesRequired = calcRequiredPages(size);
	_totalUsedMemory += (size_t)numPagesRequired * _pageSize;
    _totalNumAllocations += 1;

    return ptr;
}

bool PageAllocator::deallocate(void* p, size_t size)
{
	FS_ASSERT(p != nullptr);
	FS_ASSERT(size > 0);

    if(munmap(p, size))
    {
        FS_ASSERT_MSG_FORMATTED(false, "Failed to munmap pointer. errno: %i.", errno);
        return false;
    }

    _totalNumAllocations -= 1;
    _totalUsedMemory -= size;

    return true;
}

bool PageAllocator::deallocate(void*)
{
	FS_ASSERT(!"On linux you must use PageAllocator::deallocate(p, size)");
    // To improve this, a map of all allocations and their sizes would need to be kept.
    return false;
}

void PageAllocator::clear()
{
	FS_ASSERT(!"PageAllocator cannot be cleared.");
	// To implement this, the allocator would need to keep a list
	// of allocations and deallocate each one.
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
