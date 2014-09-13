#include <boost/test/unit_test.hpp>

#include <vector>
#include <map>

#include "fstest.h"
#include "fscore.h"

using namespace fs;

BOOST_AUTO_TEST_SUITE(core)
BOOST_AUTO_TEST_SUITE(memory)

const size_t allocatorSize = 1024;
const size_t largeAllocationSize = 128;
const size_t smallAllocationSize = 32;
// const size_t tinyAllocationSize = 4;
const size_t defaultAlignment = 8;
const size_t pageSize = 4096;


struct MemoryArenaFixture
{
    template<class Alloc>
    using BasicArena = MemoryArena<Allocator<Alloc, NoAllocationHeader>,
                                   SingleThread, NoBoundsChecking, NoMemoryTracking, NoMemoryTagging>;

    template<class HeaderPolicy>
    using ArenaWithHeader = MemoryArena<Allocator<LinearAllocator, HeaderPolicy>,
                                        SingleThread, NoBoundsChecking, NoMemoryTracking, NoMemoryTagging>;

    template<class BoundsCheckingPolicy>
    using ArenaWithBoundsChecking = MemoryArena<Allocator<LinearAllocator, AllocationHeaderU32>,
                                                SingleThread, BoundsCheckingPolicy, NoMemoryTracking, NoMemoryTagging>;

    using ArenaWithMemoryTagging = MemoryArena<Allocator<StackAllocatorBottom, AllocationHeaderU32>,
                                               SingleThread, NoBoundsChecking, NoMemoryTracking, MemoryTagging>;

    using ArenaWithExtendedTracking = MemoryArena<Allocator<StackAllocatorBottomGrowable, AllocationHeaderU32>,
                                                  SingleThread, NoBoundsChecking, ExtendedMemoryTracking, NoMemoryTagging>;

    using ArenaWithFullTracking = MemoryArena<Allocator<StackAllocatorBottomGrowable, AllocationHeaderU32>,
                                              SingleThread, SimpleBoundsChecking, FullMemoryTracking, MemoryTagging>;

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
        SourceInfo info(__FILE__, __LINE__);
        void* ptr = arena.allocate(smallAllocationSize, defaultAlignment, info);
        BOOST_REQUIRE(ptr);
        arena.reset();
    }

    template<class BoundsCheckingPolicy, class Area>
    void testBoundsChecking(Area& area)
    {
        SourceInfo info(__FILE__, __LINE__);
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
        // dont include header as part of the size.
        size -= (BoundsCheckingPolicy::SIZE_FRONT + BoundsCheckingPolicy::SIZE_BACK + sizeof(u32));

        // In addition to checking the bounds manually, also run it through the policy.
        // boundsChecker should not assert.
        BoundsCheckingPolicy boundsChecker;
        boundsChecker.checkFront((void*)(as_char - BoundsCheckingPolicy::SIZE_FRONT));
        boundsChecker.checkBack((void*)(as_char + size));

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

    SourceInfo info(__FILE__, __LINE__);

    HeapArea heapArea(allocatorSize);

    {
        ArenaWithHeader<AllocationHeaderU8> arena_u8(heapArea);
        as_void = arena_u8.allocate(smallAllocationSize, defaultAlignment, info);
        BOOST_CHECK(as_void);
        BOOST_CHECK(*(as_u8-1) == smallAllocationSize + AllocationHeaderU8::SIZE);
    }
    {
        ArenaWithHeader<AllocationHeaderU16> arena_u16(heapArea);
        as_void = arena_u16.allocate(smallAllocationSize, defaultAlignment, info);
        BOOST_CHECK(as_void);
        BOOST_CHECK(*(as_u16-1) == smallAllocationSize + AllocationHeaderU16::SIZE);
    }
    {
        ArenaWithHeader<AllocationHeaderU32> arena_u32(heapArea);
        as_void = arena_u32.allocate(smallAllocationSize, defaultAlignment, info);
        BOOST_CHECK(as_void);
        BOOST_CHECK(*(as_u32-1) == smallAllocationSize + AllocationHeaderU32::SIZE);
    }
    {
        ArenaWithHeader<AllocationHeaderU64> arena_u64(heapArea);
        as_void = arena_u64.allocate(smallAllocationSize, defaultAlignment, info);
        BOOST_CHECK(as_void);
        BOOST_CHECK(*(as_u64-1) == smallAllocationSize + AllocationHeaderU64::SIZE);
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

BOOST_AUTO_TEST_CASE(arena_with_memory_tagging)
{
    SourceInfo info(__FILE__, __LINE__);

    HeapArea area(allocatorSize);
    ArenaWithMemoryTagging arena(area);
    u32 value = 0;

    void* ptr1 = arena.allocate(4, defaultAlignment, info);
    BOOST_REQUIRE(ptr1);
    BOOST_CHECK(*(static_cast<u32*>(ptr1)) == ALLLOCATED_TAG_PATTERN);

    void* ptr2 = arena.allocate(8, defaultAlignment, info);
    BOOST_REQUIRE(ptr2);
    BOOST_CHECK(*(static_cast<u32*>(ptr2)) == ALLLOCATED_TAG_PATTERN);
    BOOST_CHECK(*(static_cast<u32*>(ptr2) + 1) == ALLLOCATED_TAG_PATTERN);

    void* ptr3 = arena.allocate(6, defaultAlignment, info);
    BOOST_REQUIRE(ptr3);
    value = *(static_cast<u32*>(ptr3));
    BOOST_CHECK(*(static_cast<u32*>(ptr3)) == ALLLOCATED_TAG_PATTERN);
    value = *(static_cast<u32*>(ptr3) + 1);
    BOOST_CHECK((value & 0xFFFF) == (ALLLOCATED_TAG_PATTERN & 0xFFFF));
    BOOST_CHECK((value & 0xFFFF0000) != (ALLLOCATED_TAG_PATTERN & 0xFFFF0000));

    arena.free(ptr3);
    value = *(static_cast<u32*>(ptr3));
    BOOST_CHECK(*(static_cast<u32*>(ptr3)) == DEALLLOCATED_TAG_PATTERN);
    value = *(static_cast<u32*>(ptr3) + 1);
    BOOST_CHECK((value & 0xFFFF) == (DEALLLOCATED_TAG_PATTERN & 0xFFFF));
    BOOST_CHECK((value & 0xFFFF0000) != (DEALLLOCATED_TAG_PATTERN & 0xFFFF0000));

    arena.free(ptr2);
    BOOST_CHECK(*(static_cast<u32*>(ptr2)) == DEALLLOCATED_TAG_PATTERN);
    BOOST_CHECK(*(static_cast<u32*>(ptr2) + 1) == DEALLLOCATED_TAG_PATTERN);

    arena.free(ptr1);
    BOOST_CHECK(*(static_cast<u32*>(ptr1)) == DEALLLOCATED_TAG_PATTERN);

}

BOOST_AUTO_TEST_CASE(debug_arena)
{
    SourceInfo info(__FILE__, __LINE__);

    DebugArena* pArena = fs::memory::getDebugArena();
    void* ptr = pArena->allocate(pageSize, defaultAlignment, info);
    BOOST_REQUIRE(ptr);
    pArena->free(ptr);

    ptr = pArena->allocate(largeAllocationSize, defaultAlignment, info);
    BOOST_REQUIRE(ptr);
    pArena->reset();
}

BOOST_AUTO_TEST_CASE(stl_allocator)
{
    // TODO: Determine how to properly test all aspects of an stl allocator.
    {
        StlAllocator<u32, DebugArena> allocator_vector(fs::memory::getDebugArena());
        std::vector<u32, StlAllocator<u32, DebugArena>> vector(allocator_vector);

        vector.push_back(1);
        vector.push_back(2);
        vector.push_back(3);
        vector.push_back(4);

        vector.erase(vector.begin());

        vector.clear();

        StlAllocator<u32, DebugArena> allocator_map(fs::memory::getDebugArena());
        std::map<u32, u32, std::less<u32>, StlAllocator<u32, DebugArena>> map(allocator_map);

        map.insert(std::pair<u32, u32>(1,1));
        map.clear();
    }
    fs::memory::getDebugArena()->reset();
}

// BOOST_AUTO_TEST_CASE(temp_test_arena_leak_report)
// {
//     SourceInfo info(__FILE__, __LINE__);
//
//     GrowableHeapArea area(pageSize * 32, pageSize * 64);
//     ArenaWithFullTracking arena(area, "trackingTest");
//
//     arena.logTrackerReport();
//
//     arena.allocate(smallAllocationSize, defaultAlignment, info);
//     arena.allocate(smallAllocationSize, defaultAlignment, info);
//     arena.allocate(smallAllocationSize, defaultAlignment, info);
//     arena.allocate(pageSize * 20, defaultAlignment, info);
//     arena.allocate(smallAllocationSize, defaultAlignment, info);
//     arena.allocate(smallAllocationSize, defaultAlignment, info);
//     arena.allocate(pageSize, defaultAlignment, info);
//     arena.logTrackerReport();
//
//     arena.reset();
// }

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
