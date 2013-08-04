#include <boost/test/unit_test.hpp>

#include <core/poolallocator.h>
#include <core/types.h>
#include <exception>

#define DEFAULT_MEM_SIZE  64 // bytes

using namespace fissura;

BOOST_AUTO_TEST_SUITE(core)
BOOST_AUTO_TEST_SUITE(allocation)

struct poolallocator_fixture
{
	poolallocator_fixture()
	{
		pAllocator = nullptr;
		pMemory = nullptr;
		resizeMemory(DEFAULT_MEM_SIZE);
	}

	~poolallocator_fixture()
	{
		pAllocator->clear();
		resizeMemory(0);
	}

	void resizeMemory(u32 size)
	{
		delete pAllocator;
		delete pMemory;

		currentMemorySize = size;
		if(size > 0)
		{
			pMemory = new u8[size];
			pAllocator = new PoolAllocator(nullptr, sizeof(u64), __alignof(u64), size, pMemory);
		}
	}

	u32 currentMemorySize;
	PoolAllocator* pAllocator;
	void* pMemory;
};

BOOST_FIXTURE_TEST_SUITE(pool_allocator, poolallocator_fixture)

BOOST_AUTO_TEST_CASE(valid_construction)
{
	BOOST_CHECK(pAllocator->getTotalNumAllocations() == 0);
	BOOST_CHECK(pAllocator->getTotalUsedMemory() == 0);
}

BOOST_AUTO_TEST_CASE(allocate_and_deallocate_single)
{
	void* pAllocation = pAllocator->allocateSingle();
	BOOST_CHECK(pAllocator->getTotalNumAllocations() == 1);
	BOOST_CHECK(pAllocator->getTotalUsedMemory() == sizeof(u64));
	BOOST_CHECK(pAllocation >= pMemory);
	BOOST_CHECK((uptr)pAllocation < (uptr)pMemory + DEFAULT_MEM_SIZE);

	pAllocator->deallocate(pAllocation);
	pAllocation = nullptr;
	BOOST_CHECK(pAllocator->getTotalNumAllocations() == 0);
	BOOST_CHECK(pAllocator->getTotalUsedMemory() == 0);
}

BOOST_AUTO_TEST_CASE(allocate_and_deallocate_many)
{
	void* pAllocation1 = pAllocator->allocateSingle();
	void* pAllocation2 = pAllocator->allocateSingle();
	void* pAllocation3 = pAllocator->allocateSingle();
	BOOST_CHECK(pAllocator->getTotalNumAllocations() == 3);
	BOOST_CHECK(pAllocator->getTotalUsedMemory() == sizeof(u64) * 3);

	pAllocator->deallocate(pAllocation2);
	pAllocator->deallocate(pAllocation3);
	pAllocator->deallocate(pAllocation1);
	
	BOOST_CHECK(pAllocator->getTotalNumAllocations() == 0);
	BOOST_CHECK(pAllocator->getTotalUsedMemory() == 0);

	pAllocator->allocateSingle();
	BOOST_CHECK(pAllocator->getTotalNumAllocations() == 1);
	BOOST_CHECK(pAllocator->getTotalUsedMemory() == sizeof(u64));
}

BOOST_AUTO_TEST_CASE(allocate_out_of_memory)
{
	resizeMemory(8);

	pAllocator->allocateSingle();

	void* pAllocation = nullptr;
	BOOST_REQUIRE_THROW(pAllocation = pAllocator->allocateSingle(), fissura::assert_exception);
	BOOST_REQUIRE(pAllocation == nullptr);

	BOOST_CHECK(pAllocator->getTotalNumAllocations() == 1);
	BOOST_CHECK(pAllocator->getTotalUsedMemory() == 8);
}

BOOST_AUTO_TEST_CASE(allocate_invalid_sizes)
{
	BOOST_REQUIRE_THROW(pAllocator->allocate(8, 4), fissura::assert_exception);
	BOOST_REQUIRE_THROW(pAllocator->allocate(7, 4), fissura::assert_exception);
	BOOST_REQUIRE_THROW(pAllocator->allocate(4, 8), fissura::assert_exception);
	BOOST_REQUIRE_THROW(pAllocator->allocate(4, 7), fissura::assert_exception);
	BOOST_REQUIRE_THROW(pAllocator->allocate(4, 4), fissura::assert_exception);
	BOOST_REQUIRE_THROW(pAllocator->allocate(7, 7), fissura::assert_exception);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()