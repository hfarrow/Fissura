#include <boost/test/unit_test.hpp>

#include <core/heapallocator.h>
#include <core/types.h>
#include <exception>

#define DEFAULT_MEM_SIZE  64 * 1024 // bytes

using namespace fissura;

BOOST_AUTO_TEST_SUITE(core)
BOOST_AUTO_TEST_SUITE(allocation)

struct heapallocator_fixture
{
	heapallocator_fixture()
	{
		pAllocator = nullptr;
		pMemory = nullptr;
		resizeMemory(DEFAULT_MEM_SIZE);
	}

	~heapallocator_fixture()
	{
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
			pAllocator = new HeapAllocator(size, pMemory);
		}
	}

	u32 currentMemorySize;
	HeapAllocator* pAllocator;
	void* pMemory;
};

BOOST_FIXTURE_TEST_SUITE(heapallocator, heapallocator_fixture)

BOOST_AUTO_TEST_CASE(valid_construction)
{
	BOOST_CHECK(pAllocator->getTotalNumAllocations() == 0);
	// Heap bookkeeping has an initial cost. Heap will be using more
	// than 0 bytes.
	BOOST_CHECK(pAllocator->getTotalUsedMemory() > 0);
}

BOOST_AUTO_TEST_CASE(allocate_and_deallocate)
{
	void* p = pAllocator->allocate(4, 4);
	BOOST_REQUIRE(p != nullptr);
	BOOST_CHECK(pAllocator->getTotalNumAllocations() == 1);

	pAllocator->deallocate(p);
	BOOST_CHECK(pAllocator->getTotalNumAllocations() == 0);
}

BOOST_AUTO_TEST_CASE(allocate_and_deallocate_large)
{
	void* p = pAllocator->allocate(DEFAULT_MEM_SIZE / 2, 4);
	BOOST_REQUIRE(p != nullptr);
	BOOST_CHECK(pAllocator->getTotalNumAllocations() == 1);

	pAllocator->deallocate(p);
	BOOST_CHECK(pAllocator->getTotalNumAllocations() == 0);
}

BOOST_AUTO_TEST_CASE(allocate_out_of_memory)
{
	void* p = nullptr;
	BOOST_REQUIRE_THROW(p = pAllocator->allocate(DEFAULT_MEM_SIZE * 2, 4), std::exception);

	// In Release, p will still be allocated. This is because dlmalloc will expand
	// the mspace if more space is needed. Fissura will assert to notify that
	// this allocation excedes the initial budget of the allocator.
	//BOOST_REQUIRE(p == nullptr);
	//BOOST_CHECK(pAllocator->getTotalNumAllocations() == 0);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()