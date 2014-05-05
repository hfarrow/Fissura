#include <iostream>
#include <map>
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "fscore.h"

using namespace fs;
using namespace std;


using SdlArena = MemoryArena<Allocator<HeapAllocator, AllocationHeaderU32>,
                             MultiThread<MutexPrimitive>,
                             SimpleBoundsChecking,
                             FullMemoryTracking,
                             MemoryTagging>;

class SdlArenaAdatper : public IArenaAdapter 
{
public:
    SdlArenaAdatper(SdlArena* pArena) :
        _pArena(pArena)
    {
    }

    virtual void* allocate(size_t size, size_t alignment, const SourceInfo& sourceInfo) override
    {
        FS_PRINT("SdlArenaAdatper allocate");
        void* ptr = _pArena->allocate(size, alignment, sourceInfo);
        FS_PRINT("allocated ptr = " << ptr);
        return ptr;
    }

    virtual void free(void* ptr) override
    {
        FS_PRINT("SdlArenaAdatper free " << ptr);
        _pArena->free(ptr);
    }

private:
    SdlArena* _pArena;
};

int main( int, char **)
{
    HeapArea sdlArea(FS_SIZE_OF_MB * 32);
    SdlArena arena(sdlArea, "SdlArena");

    SdlArenaAdatper wrapper(&arena);
    fs::setSdlArena(&wrapper);

    void* ptr = SDL_malloc(32);
    
    FS_PRINT("========== Log tracker report before allocation freed.");
    arena.logTrackerReport();

    SDL_free(ptr);

    FS_PRINT("========== Log tracker report after allocation freed.");
    arena.logTrackerReport();

    return 0;
}
