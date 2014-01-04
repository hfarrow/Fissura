#include <execinfo.h>
#include <stdio.h>
#include <string.h>

#include "fscore/allocators/trace_allocator.h"
#include "fscore/allocators/heap_allocator.h"
#include "fscore/allocators/stl_allocator.h"
#include "fscore/utils/globals.h"
#include "fscore/memory/memory.h"
#include "fscore/debugging/assert.h"
#include "fscore/utils/utils.h"
#include "fscore/memory/new.h"


using namespace fs;

TraceAllocator::TraceAllocator(const fswchar* const  pName, Allocator& allocator)
	: ProxyAllocator(pName, allocator),
    _allocationMapAllocator(*Memory::getDefaultDebugAllocator())
{
    FS_ALLOCATE_UNIQUE(AllocationMap, _pAllocationMap, 
            Memory::getDefaultDebugAllocator(), std::less<uptr>(), _allocationMapAllocator);
}

TraceAllocator::~TraceAllocator()
{
    reportMemoryLeaks();
	_pAllocationMap->clear();
}

void TraceAllocator::reportMemoryLeaks()
{
	if(_pAllocationMap->size() > 0)
	{
		for(auto it = _pAllocationMap->begin(); it != _pAllocationMap->end(); ++it)
		{
            auto pStackTrace = getCaller(&it->second);
            if(pStackTrace)
            {
                std::string name;
                const fswchar* wname = getName();
                if(wname)
                {
                    std::wstring ws(wname);
                    name = std::string(ws.begin(), ws.end());
                }
                else
                {
                    name = "Unkown";
                }
			    FS_ASSERT_MSG_FORMATTED(false, boost::format("TraceAllocator(%s) memory leak:\n%s") % name % pStackTrace);
            }
		}
	}
}

void* TraceAllocator::allocate(size_t size, u8 alignment)
{
	void* pAllocation = ProxyAllocator::allocate(size, alignment);

	if(pAllocation != nullptr)
	{
		AllocationInfo info;
		getStackTrace(&info, pAllocation);
		_pAllocationMap->insert(std::make_pair((uptr)pAllocation, info));
	}

	return pAllocation;
}

bool TraceAllocator::deallocate(void* p)
{	
	if(!ProxyAllocator::deallocate(p))
		return false;

	_pAllocationMap->erase((uptr)p);
	return true;
}

void TraceAllocator::getStackTrace(AllocationInfo* pInfo, void*)
{
    pInfo->numFrames = backtrace(pInfo->frames, pInfo->maxStackFrames);
}

const char* TraceAllocator::getCaller(const AllocationInfo* const pInfo) const
{
    const size_t bufferSize = 4096;
	static char pBuffer[bufferSize];

    char** pStrings = backtrace_symbols(pInfo->frames, pInfo->numFrames);

    char* pCurrent = pBuffer;
    for(u32 i = 0; i < pInfo->numFrames; i++)
    {
        char* pStackSymbol = *(pStrings + i);
        strncpy(pCurrent, pStackSymbol, bufferSize - (pCurrent - pBuffer));
        pCurrent += strlen(pStackSymbol);

        if(pCurrent < pBuffer + bufferSize)
        {
            *pCurrent = '\n';
            pCurrent++;
        }
    }
    
    free(pStrings);

    return pBuffer;
}

