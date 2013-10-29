#include <exception>

#include <boost/test/unit_test.hpp>

#include "fstest.h"
#include "fscore.h"

#define DEFAULT_MEM_SIZE  64 * 1024 // bytes
#define DEBUG_MEM_SIZE 1048576 // 1mb

using namespace fs;

BOOST_AUTO_TEST_SUITE(core)
BOOST_AUTO_TEST_SUITE(allocators)

struct pageallocator_fixture
{
	pageallocator_fixture()
	{
		pAllocator = nullptr;
		pMemory = nullptr;
		pDebugMemory = nullptr;
        gpDebugHeap = nullptr;
		resizeMemory(DEFAULT_MEM_SIZE);
	}

	~pageallocator_fixture()
	{
		resizeMemory(0);
	}

	void resizeMemory(u32 size)
	{
		delete pAllocator;
		delete[] (u8*)pMemory;
		delete gpDebugHeap;
        gpDebugHeap = nullptr;
		delete[] (u8*)pDebugMemory;

		currentMemorySize = size;
		if(size > 0)
		{
			pDebugMemory = new u8[DEBUG_MEM_SIZE]; // 1mb
			gpDebugHeap = new HeapAllocator(nullptr, DEBUG_MEM_SIZE, pDebugMemory);
			pMemory = new u8[size];
			pAllocator = new PageAllocator(nullptr);
		}
	}

	size_t currentMemorySize;
	PageAllocator* pAllocator;
	void* pMemory;
	void* pDebugMemory;
};

BOOST_FIXTURE_TEST_SUITE(page_allocator, pageallocator_fixture)

BOOST_AUTO_TEST_CASE(can_deallocate_is_true)
{
	BOOST_CHECK(pAllocator->canDeallocate());
}

BOOST_AUTO_TEST_CASE(calc_required_pages)
{
	BOOST_CHECK(pAllocator->calcRequiredPages(pAllocator->getPageSize() - 1) == 1);
	BOOST_CHECK(pAllocator->calcRequiredPages(pAllocator->getPageSize()) == 1);
	BOOST_CHECK(pAllocator->calcRequiredPages(pAllocator->getPageSize() + 1) == 2);
	BOOST_CHECK(pAllocator->calcRequiredPages(pAllocator->getPageSize() * 2) == 2);
	BOOST_CHECK(pAllocator->calcRequiredPages(pAllocator->getPageSize() * 2  + 1) == 3);
}

BOOST_AUTO_TEST_CASE(allocate_and_deallocate)
{
	void* p = pAllocator->allocate(pAllocator->getPageSize(), 0);
	BOOST_REQUIRE(p != nullptr);
	BOOST_CHECK(pAllocator->getTotalNumAllocations() == 1);
	BOOST_CHECK(pAllocator->getTotalUsedMemory() == pAllocator->getPageSize());

	pAllocator->deallocate(p, pAllocator->getPageSize());
	BOOST_CHECK(pAllocator->getTotalNumAllocations() == 0);
	BOOST_CHECK(pAllocator->getTotalUsedMemory() == 0);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
