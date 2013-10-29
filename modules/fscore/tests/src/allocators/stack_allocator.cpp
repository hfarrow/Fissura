#include <exception>

#include <boost/test/unit_test.hpp>

#include "fstest.h"
#include "fscore.h"

#define DEFAULT_STACK_MEM_SIZE  256 // bytes

using namespace fs;

BOOST_AUTO_TEST_SUITE(core)
BOOST_AUTO_TEST_SUITE(allocators)

struct stackallocator_fixture
{
	stackallocator_fixture()
	{
		pStack = nullptr;
		pReverseStack = nullptr;
		pStackData = nullptr;
		resizeStack(DEFAULT_STACK_MEM_SIZE);
	}

	~stackallocator_fixture()
	{
		pStack->clear();
		resizeStack(0);
	}

	void resizeStack(u32 size)
	{
		delete pStack;
		delete pReverseStack;
		delete[] (u8*)pStackData;

		currentStackSize = size;
		if(size > 0)
		{
			pStackData = new u8[size];
			pStack = new StackAllocator(nullptr, size, pStackData);
			pReverseStack = new StackAllocator(nullptr, size, pStackData, false);
		}
	}

	u32 currentStackSize;
	StackAllocator* pStack;
	StackAllocator* pReverseStack;
	void* pStackData;
};

BOOST_FIXTURE_TEST_SUITE(stack_allocator, stackallocator_fixture)

BOOST_AUTO_TEST_CASE(valid_construction)
{
	BOOST_CHECK(pStack->getTotalNumAllocations() == 0);
	BOOST_CHECK(pStack->getTotalUsedMemory() == 0);
	
	auto marker = pStack->getMarker();
	BOOST_CHECK((uptr)marker.position == (uptr)pStackData);
	BOOST_CHECK(marker.allocationIndex == 0);
}

BOOST_AUTO_TEST_CASE(allocate_memory_available)
{
	resizeStack(32);

	u32 totalAllocations = currentStackSize / sizeof(u32);
	for(u32 i = 0; i < totalAllocations; ++i)
	{
		void* allocation = pStack->allocate(sizeof(u32), __alignof(u32));
		BOOST_CHECK(allocation != nullptr);
	}

	BOOST_CHECK(pStack->getTotalNumAllocations() == totalAllocations);
	BOOST_CHECK(pStack->getTotalUsedMemory() == totalAllocations * sizeof(u32));

	void* pAllocation = nullptr;
    FS_REQUIRE_ASSERT([&](){pAllocation = pStack->allocate(sizeof(u32), __alignof(u32));});
	BOOST_CHECK(pAllocation == nullptr);

	auto marker = pStack->getMarker();
	BOOST_CHECK(marker.allocationIndex == totalAllocations);
	BOOST_CHECK(marker.position == (size_t)pStackData + pStack->getTotalUsedMemory());
}

BOOST_AUTO_TEST_CASE(allocate_out_of_memory)
{
	auto marker = pStack->getMarker();

	void* pAllocation = nullptr;
    FS_REQUIRE_ASSERT([&](){pAllocation = pStack->allocate(DEFAULT_STACK_MEM_SIZE + 4, 4);});
	BOOST_CHECK(pAllocation == nullptr);

	auto newMarker = pStack->getMarker();
	BOOST_CHECK(newMarker.position == marker.position);
	BOOST_CHECK(newMarker.allocationIndex == marker.allocationIndex);
	
}

BOOST_AUTO_TEST_CASE(deallocate_not_allowed)
{
	void* pAllocation = pStack->allocate(DEFAULT_STACK_MEM_SIZE, 4);
	auto end = pStack->getMarker();
	BOOST_CHECK(pAllocation != nullptr);

    FS_REQUIRE_ASSERT([&](){pStack->deallocate(pAllocation);});
	auto marker = pStack->getMarker();
	BOOST_CHECK(marker.position == end.position);
	BOOST_CHECK(marker.allocationIndex == 1);
	BOOST_CHECK(pStack->getTotalUsedMemory() == DEFAULT_STACK_MEM_SIZE);
	BOOST_CHECK(pStack->getTotalNumAllocations() == 1);
}

BOOST_AUTO_TEST_CASE(deallocate_to_begin_marker)
{
	auto begin = pStack->getMarker();
	void* pAllocation = pStack->allocate(DEFAULT_STACK_MEM_SIZE, 4);
	BOOST_CHECK(pAllocation != nullptr);

	pStack->deallocateToMarker(begin);
	BOOST_CHECK(pStack->getTotalUsedMemory() == 0);
	BOOST_CHECK(pStack->getTotalNumAllocations() == 0);

	auto marker = pStack->getMarker();
	BOOST_CHECK(marker.position == begin.position);
	BOOST_CHECK(marker.allocationIndex == begin.allocationIndex);
}

BOOST_AUTO_TEST_CASE(deallocate_to_middle_marker)
{
	pStack->allocate(DEFAULT_STACK_MEM_SIZE/2, 4);
	auto middle = pStack->getMarker();
	pStack->allocate(DEFAULT_STACK_MEM_SIZE/2, 4);

	pStack->deallocateToMarker(middle);
	BOOST_CHECK(pStack->getTotalUsedMemory() == DEFAULT_STACK_MEM_SIZE/2);
	BOOST_CHECK(pStack->getTotalNumAllocations() == 1);

	auto marker = pStack->getMarker();
	BOOST_CHECK(marker.position == middle.position);
	BOOST_CHECK(marker.allocationIndex == middle.allocationIndex);
}

BOOST_AUTO_TEST_CASE(deallocate_to_end_marker)
{
	void* pAllocation = pStack->allocate(DEFAULT_STACK_MEM_SIZE, 4);
	auto end = pStack->getMarker();
	BOOST_CHECK(pAllocation != nullptr);

	pStack->deallocateToMarker(end);
	BOOST_CHECK(pStack->getTotalUsedMemory() == DEFAULT_STACK_MEM_SIZE);
	BOOST_CHECK(pStack->getTotalNumAllocations() == 1);

	auto marker = pStack->getMarker();
	BOOST_CHECK(marker.position == end.position);
	BOOST_CHECK(marker.allocationIndex == end.allocationIndex);
}

BOOST_AUTO_TEST_CASE(deallocate_to_old_invalid_marker)
{
	auto begin = pStack->getMarker();
	void* pAllocation = pStack->allocate(DEFAULT_STACK_MEM_SIZE, 4);
	auto end = pStack->getMarker();
	BOOST_CHECK(pAllocation != nullptr);

	pStack->deallocateToMarker(begin);
    FS_REQUIRE_ASSERT([&](){pStack->deallocateToMarker(end);});
	BOOST_CHECK(pStack->getTotalUsedMemory() == 0);
	BOOST_CHECK(pStack->getTotalNumAllocations() == 0);

	auto marker = pStack->getMarker();
	BOOST_CHECK(marker.position == begin.position);
	BOOST_CHECK(marker.allocationIndex == begin.allocationIndex);
}

BOOST_AUTO_TEST_CASE(clear_with_no_allocations_made)
{
	auto begin = pStack->getMarker();
	pStack->clear();
	BOOST_CHECK(pStack->getTotalUsedMemory() == 0);
	BOOST_CHECK(pStack->getTotalNumAllocations() == 0);

	auto marker = pStack->getMarker();
	BOOST_CHECK(marker.position == begin.position);
	BOOST_CHECK(marker.allocationIndex == begin.allocationIndex);
}

BOOST_AUTO_TEST_CASE(clear_with_allocations_made)
{
	auto begin = pStack->getMarker();
	pStack->allocate(DEFAULT_STACK_MEM_SIZE, 4);
	pStack->clear();
	BOOST_CHECK(pStack->getTotalUsedMemory() == 0);
	BOOST_CHECK(pStack->getTotalNumAllocations() == 0);

	auto marker = pStack->getMarker();
	BOOST_CHECK(marker.position == begin.position);
	BOOST_CHECK(marker.allocationIndex == begin.allocationIndex);
}

BOOST_AUTO_TEST_CASE(allocate_memory_downwards)
{
	u32 allocationSize = 32;

	auto begin = pReverseStack->getMarker();
	BOOST_CHECK(begin.allocationIndex == 0);
	BOOST_CHECK(begin.position == (size_t)pStackData + currentStackSize);

	pReverseStack->allocate(allocationSize, 4);
	auto marker = pReverseStack->getMarker();
	BOOST_CHECK(marker.allocationIndex == 1);
	BOOST_CHECK(marker.position == (size_t)pStackData + currentStackSize - allocationSize);
	BOOST_CHECK(pReverseStack->getTotalUsedMemory() == allocationSize);
	BOOST_CHECK(pReverseStack->getTotalNumAllocations() == 1);

	pReverseStack->deallocateToMarker(begin);
	BOOST_CHECK(pReverseStack->getTotalUsedMemory() == 0);
	BOOST_CHECK(pReverseStack->getTotalNumAllocations() == 0);

	begin = pReverseStack->getMarker();
	BOOST_CHECK(begin.allocationIndex == 0);
	BOOST_CHECK(begin.position == (size_t)pStackData + currentStackSize);
}

BOOST_AUTO_TEST_CASE(allocate_out_of_memory_downwards)
{
	auto begin = pReverseStack->getMarker();

	void* pAllocation = nullptr;
    FS_REQUIRE_ASSERT([&](){pAllocation = pReverseStack->allocate(DEFAULT_STACK_MEM_SIZE + 4, 4);});
	BOOST_CHECK(pAllocation == nullptr);

	auto newMarker = pReverseStack->getMarker();
	BOOST_CHECK(newMarker.position == begin.position);
	BOOST_CHECK(newMarker.allocationIndex == begin.allocationIndex);
}

BOOST_AUTO_TEST_CASE(allocate_downwards_and_clear)
{
	auto begin = pReverseStack->getMarker();

	pReverseStack->allocate(DEFAULT_STACK_MEM_SIZE, 4);
	pReverseStack->clear();

	auto newMarker = pReverseStack->getMarker();
	BOOST_CHECK(newMarker.position == begin.position);
	BOOST_CHECK(newMarker.allocationIndex == begin.allocationIndex);
	BOOST_CHECK(pReverseStack->getTotalNumAllocations() == 0);
	BOOST_CHECK(pReverseStack->getTotalUsedMemory() == 0);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
