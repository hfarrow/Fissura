#include <boost/test/unit_test.hpp>

#include "fstest.h"
#include "fscore.h"

using namespace fs;

BOOST_AUTO_TEST_SUITE(core)
BOOST_AUTO_TEST_SUITE(memory)

struct PageAllocatorFixture
{
    PageAllocatorFixture()
    {

    }

    ~PageAllocatorFixture()
    {

    }
};

BOOST_FIXTURE_TEST_SUITE(page_allocator, PageAllocatorFixture)

BOOST_AUTO_TEST_CASE(allocate_and_free)
{
    PageAllocator allocator;

    void* ptr = allocator.allocate(VirtualMemory::getPageSize(), 0, 0);
    BOOST_REQUIRE(ptr);
    allocator.free(ptr, VirtualMemory::getPageSize());

    // By default will allocate up to page size.
    ptr = allocator.allocate(32, 0, 0);
    BOOST_REQUIRE(ptr);
    allocator.free(ptr, 32);

    FS_REQUIRE_ASSERT([&](){allocator.allocate(VirtualMemory::getPageSize(), 1, 0);});
    FS_REQUIRE_ASSERT([&](){allocator.allocate(VirtualMemory::getPageSize(), 0, 1);});
    FS_REQUIRE_ASSERT([&](){allocator.allocate(0, 0, 0);});
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
