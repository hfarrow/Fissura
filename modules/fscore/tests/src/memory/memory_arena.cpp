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

template<class Alloc>
using BasicArena = MemoryArena<Allocator<Alloc, NoAllocationHeader>,
                      SingleThreadPolicy, NoBoundsChecking, NoMemoryTracking, NoMemoryTagging>;

template<class HeaderPolicy>
using ArenaWithHeader = MemoryArena<Allocator<LinearAllocator, HeaderPolicy>,
                                    SingleThreadPolicy, NoBoundsChecking, NoMemoryTracking, NoMemoryTagging>;

template<class BoundCheckingPolicy>
using ArenaWithBoundsChecking = MemoryArena<Allocator<LinearAllocator, AllocationHeaderU32>,
                                    SingleThreadPolicy, BoundCheckingPolicy, NoMemoryTracking, NoMemoryTagging>;

struct MemoryArenaFixture
{
    MemoryArenaFixture()
    {
    }

    ~MemoryArenaFixture()
    {
    
    }

    template<class Area, class Arena>
    void arenaInitFromArea(Area& area)
    {
        Arena arena(area);
        allocateAndFreeFromArena(arena);
    }

    template<class Arena>
    void allocateAndFreeFromArena(Arena& arena)
    {
        SourceInfo info;
        info.fileName = "testFileName";
        info.lineNumber = 1234;
        void* ptr = arena.allocate(smallAllocationSize, defaultAlignment, info);
        BOOST_REQUIRE(ptr);
        arena.reset();
    }

    template<class BoundsCheckingPolicy, class Area>
    void testBoundsChecking(Area& area)
    {
        SourceInfo info;
        info.fileName = "testFileName";
        info.lineNumber = 1234;

        ArenaWithBoundsChecking<BoundsCheckingPolicy> arena(area);

        union
        {
            void* as_void;
            char* as_char;
            u32* as_u32;
        };

        as_void = arena.allocate(smallAllocationSize, defaultAlignment, info);
        BOOST_REQUIRE(as_void);
        BOOST_REQUIRE(BoundsCheckingPolicy::SIZE_FRONT >= sizeof(u32));
        BOOST_REQUIRE(BoundsCheckingPolicy::SIZE_FRONT % sizeof(u32) == 0);
        char* front = as_char - BoundsCheckingPolicy::SIZE_FRONT;
        for(u32 i = 0; i < BoundsCheckingPolicy::SIZE_FRONT / 4; ++i)
        {
            BOOST_REQUIRE((*((u32*)front) == BOUNDS_FRONT_PATTERN));
            front += 4;
        }

        u32 size = *(as_char - BoundsCheckingPolicy::SIZE_FRONT - sizeof(u32));
        as_char = as_char + size;
        char* back = as_char;
        for(u32 i = 0; i < BoundsCheckingPolicy::SIZE_BACK / 4; ++i)
        {
            BOOST_REQUIRE((*((u32*)back) == BOUNDS_BACK_PATTERN));
            back += 4;
        }
    }
};

BOOST_FIXTURE_TEST_SUITE(memory_arena, MemoryArenaFixture)


BOOST_AUTO_TEST_CASE(arena_basic_init_and_allocate_and_free)
{
    HeapArea heapArea(allocatorSize * 20);
    StackArea<allocatorSize> stackArea;
    GrowableHeapArea growableHeap(allocatorSize / 2, allocatorSize);

#define INIT_FROM_AREA(area_type, area, arena_type) \
    arenaInitFromArea<area_type, arena_type>((area))

    INIT_FROM_AREA(HeapArea, heapArea, BasicArena<LinearAllocator>);
    INIT_FROM_AREA(StackArea<allocatorSize>, stackArea, BasicArena<LinearAllocator>);

    INIT_FROM_AREA(HeapArea, heapArea, BasicArena<StackAllocatorBottom>);
    INIT_FROM_AREA(StackArea<allocatorSize>, stackArea, BasicArena<StackAllocatorBottom>);

    INIT_FROM_AREA(HeapArea, heapArea, BasicArena<StackAllocatorTop>);
    INIT_FROM_AREA(StackArea<allocatorSize>, stackArea, BasicArena<StackAllocatorTop>);

    INIT_FROM_AREA(HeapArea, heapArea, BasicArena<HeapAllocator>);
    // Do not create heaps on the stack.
    //INIT_FROM_AREA(StackArea, stackArea, BasicArena<HeapAllocator>);
    
    arenaInitFromArea<HeapArea, BasicArena<PoolAllocatorNonGrowable<largeAllocationSize, defaultAlignment>>>(heapArea);
    arenaInitFromArea<StackArea<allocatorSize>, BasicArena<PoolAllocatorNonGrowable<largeAllocationSize, defaultAlignment>>>(stackArea);
    arenaInitFromArea<GrowableHeapArea, BasicArena<PoolAllocator<Growable, largeAllocationSize, defaultAlignment, 1>>>(growableHeap);

#undef INIT_FROM_AREA
}

BOOST_AUTO_TEST_CASE(arena_with_header)
{
    union
    {
        void* as_void;
        u8* as_u8;
        u16* as_u16;
        u32* as_u32;
        u64* as_u64;
    };

    SourceInfo info;
    info.fileName = "testFileName";
    info.lineNumber = 1234;

    HeapArea heapArea(allocatorSize);
    
    {
        ArenaWithHeader<AllocationHeaderU8> arena_u8(heapArea);
        as_void = arena_u8.allocate(smallAllocationSize, defaultAlignment, info);
        BOOST_CHECK(as_void);
        BOOST_CHECK(*(as_u8-1) == smallAllocationSize);
    }
    {
        ArenaWithHeader<AllocationHeaderU16> arena_u16(heapArea);
        as_void = arena_u16.allocate(smallAllocationSize, defaultAlignment, info);
        BOOST_CHECK(as_void);
        BOOST_CHECK(*(as_u16-1) == smallAllocationSize);
    }
    {
        ArenaWithHeader<AllocationHeaderU32> arena_u32(heapArea);
        as_void = arena_u32.allocate(smallAllocationSize, defaultAlignment, info);
        BOOST_CHECK(as_void);
        BOOST_CHECK(*(as_u32-1) == smallAllocationSize);
    }
    {
        ArenaWithHeader<AllocationHeaderU64> arena_u64(heapArea);
        as_void = arena_u64.allocate(smallAllocationSize, defaultAlignment, info);
        BOOST_CHECK(as_void);
        BOOST_CHECK(*(as_u64-1) == smallAllocationSize);
    }
}

BOOST_AUTO_TEST_CASE(arean_with_simple_bounds_checking)
{
    union
    {
        void* as_void;
        char* as_char;
        u32* as_u32;
    };

    {
        HeapArea heapArea(allocatorSize);
        testBoundsChecking<SimpleBoundsChecking>(heapArea);
    }
    {
        HeapArea heapArea(allocatorSize);
        testBoundsChecking<ExtendedBoundsChecking>(heapArea);
    }
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
