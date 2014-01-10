#include "fscore/allocators/allocator.h"
#include "fscore/utils/utils.h"
#include "fscore/memory/memory.h"
#include "fscore/memory/memory_tracker.h"

using namespace fs;

Allocator::Allocator(const fschar* const  pName)
	: _pName(pName == nullptr ? "Unkown" : pName)
{

#ifdef _DEBUG
    MemoryTracker* pTracker = Memory::getTracker();
    if(pTracker)
    {
        pTracker->registerAllocator(this);
    }
#endif
}

Allocator::~Allocator()
{
	// Subclasses should assert that all memory was released.
	// The is destructor cannot check because the functions to
	// do so are virtual.
#ifdef _DEBUG
    MemoryTracker* pTracker = Memory::getTracker();
    if(pTracker)
    {
        pTracker->unregisterAllocator(this);
    }
#endif
}

const fschar* Allocator::getName() const
{
	return _pName;
}
