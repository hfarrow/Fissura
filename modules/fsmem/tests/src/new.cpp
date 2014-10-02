#include <boost/test/unit_test.hpp>

#include "fstest.h"
#include "fscore.h"
#include "fsmem.h"

using namespace fs;

BOOST_AUTO_TEST_SUITE(core)
BOOST_AUTO_TEST_SUITE(memory)

const size_t pageSize = 4096;

struct AllocationNewFixture
{
    using StackArena = MemoryArena<Allocator<StackAllocatorBottom, AllocationHeaderU32>,
                                             SingleThread, SimpleBoundsChecking, ExtendedMemoryTracking, MemoryTagging>;

    AllocationNewFixture() :
        allocatorSize(VirtualMemory::getPageSize() * 8),
        largeAllocationSize(VirtualMemory::getPageSize()),
        smallAllocationSize(32),
        tinyAllocationSize(4),
        defaultAlignment(8)
    {
    }

    ~AllocationNewFixture()
    {

    }

    const size_t allocatorSize;
    const size_t largeAllocationSize;
    const size_t smallAllocationSize;
    const size_t tinyAllocationSize;
    const size_t defaultAlignment;

    struct TestNonPOD
    {
        TestNonPOD() : c('a') {}
        char c;
    };

    struct TestPOD
    {
        char c;
    };
};

BOOST_FIXTURE_TEST_SUITE(allocation_new, AllocationNewFixture)

BOOST_AUTO_TEST_CASE(allocate_single_and_verify)
{
    HeapArea area(pageSize * 32);
    StackArena arena(area, "allocate_single_and_verify");

    // Allocate TestPOD
    TestPOD* obj = FS_NEW(TestPOD, &arena)();
    BOOST_REQUIRE(obj);
    BOOST_CHECK(pointerUtil::alignTopAmount((uptr)obj, alignof(TestPOD)) == 0);
    FS_DELETE(obj, &arena);
    BOOST_CHECK(*reinterpret_cast<u32*>(obj) == DEALLLOCATED_TAG_PATTERN);

    // Allocate TestNonPOD
    TestNonPOD* obj2 = FS_NEW(TestNonPOD, &arena)();
    BOOST_REQUIRE(obj2);
    BOOST_CHECK(pointerUtil::alignTopAmount((uptr)obj2, alignof(TestNonPOD)) == 0);
    FS_DELETE(obj2, &arena);
    BOOST_CHECK(*reinterpret_cast<u32*>(obj2) == DEALLLOCATED_TAG_PATTERN);

    // Allocate TestPOD aligned
    TestPOD* obj3 = FS_NEW_ALIGNED(TestPOD, &arena, 16)();
    BOOST_REQUIRE(obj3);
    BOOST_CHECK(pointerUtil::alignTopAmount((uptr)obj3, 16) == 0);
    FS_DELETE(obj3, &arena);
    BOOST_CHECK(*reinterpret_cast<u32*>(obj3) == DEALLLOCATED_TAG_PATTERN);
}

BOOST_AUTO_TEST_CASE(allocate_array_and_verify)
{
    HeapArea area(pageSize * 32);
    StackArena arena(area, "allocate_array_and_verify");

    // Allocate 10 TestNonPOD and verify they were constructed
    TestNonPOD* array = FS_NEW_ARRAY(TestNonPOD[10], &arena);
    BOOST_REQUIRE(array);
    for(u32 i = 0; i < 10; ++i)
    {
        BOOST_CHECK(array[i].c == 'a');
    }
    FS_DELETE_ARRAY(array, &arena);

    // Allocate 10 TestPOD
    TestPOD* array2 = FS_NEW_ARRAY(TestPOD[10], &arena);
    BOOST_REQUIRE(array2);
    FS_DELETE_ARRAY(array2, &arena);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

