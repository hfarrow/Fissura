
#include <exception>

#include <boost/test/unit_test.hpp>

#include "fstest.h"
#include "fscore.h"

using namespace fs;

BOOST_AUTO_TEST_SUITE(core)
BOOST_AUTO_TEST_SUITE(allocators)

struct mallocallocator_fixture
{
	mallocallocator_fixture()
	{
        pAllocator = new MallocAllocator("MallocAllocator");
	}

	~mallocallocator_fixture()
	{
        delete pAllocator;
        pAllocator = nullptr;
	}

    MallocAllocator* pAllocator;
};

BOOST_FIXTURE_TEST_SUITE(malloc_allocator, mallocallocator_fixture)

BOOST_AUTO_TEST_CASE(valid_construction)
{
	BOOST_CHECK(pAllocator->getTotalNumAllocations() == 0);
	BOOST_CHECK(pAllocator->getTotalUsedMemory() == 0);
    BOOST_CHECK(pAllocator->canDeallocate());
}

BOOST_AUTO_TEST_CASE(allocate_and_deallocate)
{
    void* pAllocation = pAllocator->allocate(32, 8);
    BOOST_REQUIRE(pAllocation);
    BOOST_CHECK(pAllocator->getTotalNumAllocations() == 1);
    BOOST_CHECK(pAllocator->getTotalUsedMemory() >= 32);
    
    bool success = pAllocator->deallocate(pAllocation);
    BOOST_REQUIRE(success);
	BOOST_CHECK(pAllocator->getTotalNumAllocations() == 0);
	BOOST_CHECK(pAllocator->getTotalUsedMemory() == 0);
}


BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
