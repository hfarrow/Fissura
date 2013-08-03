#include "stdafx.h"
#include <core/pageallocator.h>
#include <core/util.h>
#include <windows.h>
#include <core/assert.h>

using namespace fissura;

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

void* PageAllocator::allocate(size_t size, u8 alignment)
{
	FS_ASSERT(size > 0);
	FS_ASSERT_MSG(alignment == 0, "PageAllocator is aligned by default. Please pass 0 for alignment.");

	u32 numPagesRequired = calcRequiredPages(size);
	void* pBase = VirtualAlloc(nullptr, size, MEM_RESERVE | MEM_COMMIT, PAGE_NOACCESS);
	if(pBase == nullptr)
	{
		FS_ASSERT(!"Page allocator failed to reserve and commit request.");
		return nullptr;
	}
	_totalUsedMemory += numPagesRequired * _pageSize;
	_totalNumAllocations += 1;

	return pBase;
}

void PageAllocator::deallocate(void* p)
{
	FS_ASSERT(p != nullptr);
	FS_ASSERT_MSG((uptr)p % _pageSize == 0, "Can only deallocate a pointer to a base page.");

	MEMORY_BASIC_INFORMATION info;
	if(VirtualQuery(p, &info, sizeof(info)) != sizeof(info))
	{
		FS_ASSERT(!"Failed query provided page base pointer.");
		return;
	}

	if(VirtualFree(p, 0, MEM_RELEASE) == 0)
	{
		DWORD error = GetLastError();
		FS_ASSERT(!"Failed to deallocate memory. See 'error' code above");
		return;
	}

	_totalNumAllocations -= 1;
	_totalUsedMemory -= info.RegionSize;
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
	u32 numPages = size / _pageSize;
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