#include "fscore/memory/extended_memory_tracking_policy.h"
#include "fscore/memory/stl_allocator.h"
#include "fscore/debugging/assert.h"
#include "fscore/debugging/utils.h"

using namespace fs;

ExtendedMemoryTracking::ExtendedMemoryTracking() :
    _nextId(0)
{
    DebugArena* pArena = fs::memory::getDebugArena();
    _profile.pAllocationMap = std::allocate_shared<AllocationMap>(AllocationMapAlloc(*pArena), *pArena);
}

void ExtendedMemoryTracking::onAllocation(void* ptr, size_t size, size_t alignment, const SourceInfo& info)
{
    (void)alignment;
    
    AllocationMapPair pair((uptr)ptr, AllocationInfo(info.fileName, info.lineNumber, size, _nextId++));
    if(!_profile.pAllocationMap->insert(pair).second)
    {
        FS_ASSERT(!"Allocation already mapped. Must be unmapped (deallocated) before being tracked again.");
    }

    _profile.numAllocations++;
    _profile.usedSize += size;
}

void ExtendedMemoryTracking::onDeallocation(void* ptr, size_t size)
{
    auto iter = _profile.pAllocationMap->find((uptr)ptr);
    FS_ASSERT_MSG(iter != _profile.pAllocationMap->end(),
                  "Could not find allocation in map. It was never tracked. Invalid free?");

    FS_ASSERT_MSG(iter->second.size == size,
                  "Size of deallocation does not match the tracked size of the allocation.");

    _profile.pAllocationMap->erase(iter);

    FS_ASSERT_MSG(_profile.numAllocations > 0, "This arena has no current allocations and therefore cannot free.");
    _profile.numAllocations--;
    _profile.usedSize -= size;
}

void ExtendedMemoryTracking::reset()
{
    _profile.numAllocations = 0;
    _profile.usedSize = 0;
    _profile.pAllocationMap->clear();
}

void FullMemoryTracking::onAllocation(void* ptr, size_t size, size_t alignment, const SourceInfo& info)
{
    (void)alignment;
    
    AllocationInfo ainfo(info.fileName, info.lineNumber, size, _nextId++);
    ainfo.numFrames = StackTraceUtil::getStackTrace(ainfo.frames, ainfo.maxStackFrames);

    AllocationMapPair pair((uptr)ptr, ainfo);
    if(!_profile.pAllocationMap->insert(pair).second)
    {
        FS_ASSERT(!"Allocation already mapped. Must be unmapped (deallocated) before being tracked again.");
    }

    _profile.numAllocations++;
    _profile.usedSize += size;
}
