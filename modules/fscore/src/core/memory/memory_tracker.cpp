#include "fscore/memory/memory_tracker.h"

#include "fscore/utils/types.h"
#include "fscore/memory/new.h"
#include "fscore/allocators/stl_allocator.h"

using namespace fs;


MemoryTracker::MemoryTracker() :
    _pAllocatorMap(nullptr),
    _allocatorMapAllocator(*Memory::getDefaultDebugAllocator()),
    _allocatorVectorAllocator(*Memory::getDefaultDebugAllocator())
{
    FS_ALLOCATE_UNIQUE(AllocatorMap, _pAllocatorMap, Memory::getDefaultDebugAllocator(),
            std::less<Allocator*>(), _allocatorMapAllocator);
}

MemoryTracker::~MemoryTracker()
{

}

void MemoryTracker::registerAllocator(Allocator* pAllocator)
{
    if(_pAllocatorMap->count(pAllocator) == 0)
    {
        AllocatorInfo info;
        info.pAllocator = pAllocator;
        info.peakUsedMemory = info.usedMemory = pAllocator->getTotalUsedMemory();
        info.peakNumAllocations = info.numAllocations = pAllocator->getTotalNumAllocations();
        _pAllocatorMap->insert(std::make_pair(pAllocator, info));
    }
}

void MemoryTracker::unregisterAllocator(Allocator* pAllocator)
{
    if(_pAllocatorMap->count(pAllocator) > 0)
    {
        _pAllocatorMap->erase(pAllocator);
    }
}

void MemoryTracker::update()
{
    if(_pAllocatorMap->size() > 0)
    {
        for(auto it = _pAllocatorMap->begin(); it != _pAllocatorMap->end(); ++it)
        {
            AllocatorInfo* pInfo = &(it->second);
            size_t currentTotalUsedMemory = pInfo->pAllocator->getTotalUsedMemory();
            u32 currentTotalNumAllocations = pInfo->pAllocator->getTotalNumAllocations();

            if(currentTotalUsedMemory > pInfo->peakUsedMemory)
            {
                pInfo->peakUsedMemory = currentTotalUsedMemory;
            }

            if(currentTotalNumAllocations > pInfo->peakNumAllocations)
            {
                pInfo->peakNumAllocations = currentTotalNumAllocations;
            }

            pInfo->usedMemory = currentTotalUsedMemory;
            pInfo->numAllocations = currentTotalNumAllocations;
        }
    }
}

// Creates copies of AllocatorInfo objects
const MemoryTracker::Report MemoryTracker::generateReport()
{
    MemoryTracker::Report report;
    report.allocators = std::allocate_shared<Vector<AllocatorInfo>>(
                        _allocatorVectorAllocator, _allocatorVectorAllocator);
    update();

    if(_pAllocatorMap->size() > 0)
    {
        for(auto it = _pAllocatorMap->begin(); it != _pAllocatorMap->end(); ++it)
        {
            report.allocators->push_back(it->second);
        }
    }
    
    return report;
}
