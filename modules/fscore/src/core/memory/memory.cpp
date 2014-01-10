#include "fscore/memory/new.h"
#include "fscore/memory/memory.h"
#include "fscore/memory/memory_tracker.h"

using namespace fs;

Allocator* Memory::_pDefaultAllocator = nullptr;
Allocator* Memory::_pDefaultDebugAllocator = nullptr;
MallocAllocator Memory::_mallocAllocator("SystemMallocAllocator");
MemoryTracker* Memory::_pTracker = nullptr;

void Memory::initTracker()
{
    FS_ASSERT_MSG(!_pTracker, "Tracker is already initialized.");
    _pTracker = FS_NEW_DEBUG(MemoryTracker)();
}

void Memory::destroyTracker()
{
    FS_ASSERT_MSG(_pTracker, "Tracker was not initialized.");
    FS_DELETE_DEBUG(_pTracker);
    _pTracker = nullptr;
}


