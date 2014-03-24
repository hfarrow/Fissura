#include <boost/test/unit_test.hpp>

#include "fstest.h"
#include "fscore.h"

using namespace fs;

BOOST_AUTO_TEST_SUITE(core)
BOOST_AUTO_TEST_SUITE(memory)

const size_t allocatorSize = 1024;
const size_t largeAllocationSize = 128;
const size_t smallAllocationSize = 32;
const size_t tinyAllocationSize = 4;
const size_t defaultAlignment = 8;
const size_t pageSize = 4096;

struct MemoryArenaFixture
{
    MemoryArenaFixture() :
        allocatorSize(VirtualMemory::getPageSize() * 2)
    {
    }

    ~MemoryArenaFixture()
    {
    
    }

    template<class Area, class Arena>
    void arenaInitFromArea(Area& area)
    {
        Arena arena(area);
    }

    template<class Arena>
    void allocateAndFreeFromArena(Arena& arena)
    {
        SourceInfo info;
        info.fileName = "testFileName";
        info.lineNumber = 1234;
        void* ptr = arena.allocate(smallAllocationSize, defaultAlignment, info);
        BOOST_REQUIRE(ptr);
        arena.free(ptr);
    }

    const size_t allocatorSize;
};

template<typename Alloc>
using BasicArena = MemoryArena<Allocator<Alloc, NoAllocationHeader>,
                      SingleThreadPolicy, NoBoundsChecking, NoMemoryTracking, NoMemoryTagging>;

BOOST_FIXTURE_TEST_SUITE(memory_arena, MemoryArenaFixture)


BOOST_AUTO_TEST_CASE(arena_basic_init_and_allocate_and_free)
{
    HeapArea heapArea(allocatorSize);
    StackArea stackArea(allocatorSize);

#define INIT_FROM_AREA(area_type, area, arena_type) \
    arenaInitFromArea<area_type, arena_type>((area))

    INIT_FROM_AREA(HeapArea, heapArea, BasicArena<LinearAllocator>);
    INIT_FROM_AREA(StackArea, stackArea, BasicArena<LinearAllocator>);

    INIT_FROM_AREA(HeapArea, heapArea, BasicArena<StackAllocatorBottom>);
    INIT_FROM_AREA(StackArea, stackArea, BasicArena<StackAllocatorBottom>);

    INIT_FROM_AREA(HeapArea, heapArea, BasicArena<StackAllocatorTop>);
    INIT_FROM_AREA(StackArea, stackArea, BasicArena<StackAllocatorTop>);

    INIT_FROM_AREA(HeapArea, heapArea, BasicArena<HeapAllocator>);
    // Do not create heaps on the stack.
    //INIT_FROM_AREA(StackArea, stackArea, BasicArena<HeapAllocator>);
    
    arenaInitFromArea<HeapArea, BasicArena<PoolAllocator<NonGrowable, largeAllocationSize, defaultAlignment, 0, pageSize>>>(heapArea);
    arenaInitFromArea<StackArea, BasicArena<PoolAllocator<NonGrowable, largeAllocationSize, defaultAlignment, 0, pageSize>>>(stackArea);
    //INIT_FROM_AREA(HeapArea, heapArea, BasicArena<PoolType>);
    //INIT_FROM_AREA(StackArea, stackArea, BasicArena<PoolType>);

#undef INIT_FROM_AREA
}

BOOST_AUTO_TEST_CASE(arena_incompatible_header_and_bounds_checker)
{
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
