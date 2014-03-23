#include <boost/test/unit_test.hpp>

#include "fstest.h"
#include "fscore.h"

using namespace fs;

BOOST_AUTO_TEST_SUITE(core)
BOOST_AUTO_TEST_SUITE(memory)

struct MemoryArenaFixture
{
    MemoryArenaFixture() :
        allocatorSize(VirtualMemory::getPageSize() * 2),
        largeAllocationSize(VirtualMemory::getPageSize()),
        smallAllocationSize(32),
        tinyAllocationSize(4),
        defaultAlignment(8)
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
    const size_t largeAllocationSize;
    const size_t smallAllocationSize;
    const size_t tinyAllocationSize;
    const size_t defaultAlignment;
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
    

    // todo:: how to test pool allocator generically?
    // move object size, alignment, and offset from ctor to template params
    // INIT_FROM_AREA(HeapArea, heapArea, BasicArena<PoolAllocator<NonGrowable>>);
    // INIT_FROM_AREA(StackArea, stackArea, BasicArena<PoolAllocator>);

#undef INIT_FROM_AREA
}

BOOST_AUTO_TEST_CASE(arena_incompatible_header_and_bounds_checker)
{
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
