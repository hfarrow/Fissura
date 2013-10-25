#include "stdafx.h"

#include <Windows.h>

#include "fscore/allocators/page_allocator_windows.h"
#include "fscore/platforms.h"
#include "fscore/globals.h"
#include "fscore/assert.h"
#include "fscore/trace.h"

using namespace fs;

PageAllocator::PageAllocator(const fschar* const pName)
	:
	_totalUsedMemory(0),
	_totalNumAllocations(0),
	Allocator(pName)
{
	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);
	_pageSize = systemInfo.dwPageSize;
}

PageAllocator::~PageAllocator()
{
	FS_ASSERT(_totalNumAllocations == 0);
	FS_ASSERT(_totalUsedMemory == 0);
}

void* PageAllocator::allocateWithFlags(size_t size, u32 allocationTypeFlag, u32 protecFlag)
{
	FS_ASSERT(size > 0);	

	u32 numPagesRequired = calcRequiredPages(size);
	void* pBase = VirtualAlloc(nullptr, size, allocationTypeFlag, protecFlag);
	if(pBase == nullptr)
	{
		FS_ASSERT(!"Page allocator failed to reserve and commit request.");
		return nullptr;
	}
	_totalUsedMemory += (size_t)numPagesRequired * _pageSize;
	_totalNumAllocations += 1;

	return pBase;
}

void* PageAllocator::allocate(size_t size, u8 alignment)
{
	FS_ASSERT_MSG(alignment == 0, "PageAllocator is aligned by default. Please pass 0 for alignment.");
	return allocateWithFlags(size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
}

bool PageAllocator::deallocate(void* p)
{
	FS_ASSERT(p != nullptr);
	FS_ASSERT_MSG((uptr)p % _pageSize == 0, "Can only deallocate a pointer to a base page.");

	MEMORY_BASIC_INFORMATION info;
	if(VirtualQuery(p, &info, sizeof(info)) != sizeof(info))
	{
		FS_ASSERT(!"Failed VirtualQuery with provided base page pointer.");
		return false;
	}

	if(VirtualFree(p, 0, MEM_RELEASE) == 0)
	{
		DWORD error = GetLastError();
		FS_ASSERT(!"Failed to deallocate memory. See 'error' code above");
		return false;
	}

	_totalNumAllocations -= 1;
	_totalUsedMemory -= info.RegionSize;

	return true;
}

bool PageAllocator::deallocate(void* ptr, size_t size)
{
    return deallocate(ptr);
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
