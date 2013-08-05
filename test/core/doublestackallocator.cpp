#include <boost/test/unit_test.hpp>

#include <core/doublestackallocator.h>
#include <core/types.h>
#include <exception>

#define DEFAULT_STACK_MEM_SIZE  256 // bytes

using namespace fs;

BOOST_AUTO_TEST_SUITE(core)
BOOST_AUTO_TEST_SUITE(allocation)

struct doublestackallocator_fixture
{
	doublestackallocator_fixture()
	{
		pStack = nullptr;
		pStackData = nullptr;
		resizeStack(DEFAULT_STACK_MEM_SIZE);
	}

	~doublestackallocator_fixture()
	{
		pStack->clear();
		resizeStack(0);
	}

	void resizeStack(u32 size)
	{
		delete pStack;
		delete pStackData;

		currentStackSize = size;
		if(size > 0)
		{
			pStackData = new u8[size];
			pStack = new DoubleStackAllocator(nullptr, size, pStackData);
		}
	}

	u32 currentStackSize;
	DoubleStackAllocator* pStack;
	void* pStackData;
};

BOOST_FIXTURE_TEST_SUITE(double_stack_allocator, doublestackallocator_fixture)

BOOST_AUTO_TEST_CASE(valid_construction)
{
	BOOST_CHECK(pStack->getTotalNumAllocations() == 0);
	BOOST_CHECK(pStack->getTotalUsedMemory() == 0);
	
	auto marker = pStack->getLower().getMarker();
	BOOST_CHECK(marker.position == (u32)pStackData);
	BOOST_CHECK(marker.allocationIndex == 0);

	marker = pStack->getUpper().getMarker();
	BOOST_CHECK(marker.position == (u32)pStackData + DEFAULT_STACK_MEM_SIZE);
	BOOST_CHECK(marker.allocationIndex == 0);
}

BOOST_AUTO_TEST_CASE(get_lower_and_lower)
{
	StackAllocator& lower = pStack->getLower();
	StackAllocator& upper = pStack->getUpper();

	auto lowerBegin = lower.getMarker();
	auto upperBegin = upper.getMarker();

	BOOST_CHECK(lowerBegin.position < upperBegin.position);
	BOOST_CHECK(lowerBegin.position == (u32)pStackData);
	BOOST_CHECK(upperBegin.position == (u32)pStackData + DEFAULT_STACK_MEM_SIZE);
}

BOOST_AUTO_TEST_CASE(allocate_and_deallocate_upper)
{
	StackAllocator& upper = pStack->getUpper();
	auto begin = upper.getMarker();
	void* pAllocation = pStack->allocateUpper(DEFAULT_STACK_MEM_SIZE, 4);
	BOOST_REQUIRE(pAllocation != nullptr);

	auto end = upper.getMarker();
	BOOST_CHECK(end.position == begin.position - DEFAULT_STACK_MEM_SIZE);

	pStack->deallocateUpper(begin);
	auto marker = upper.getMarker();
	BOOST_CHECK(marker.position == begin.position);
	BOOST_CHECK(marker.allocationIndex == begin.allocationIndex);
}

BOOST_AUTO_TEST_CASE(allocatee_and_deallocate_lower)
{
	StackAllocator& lower = pStack->getLower();
	auto begin = lower.getMarker();
	void* pAllocation = pStack->allocateLower(DEFAULT_STACK_MEM_SIZE, 4);
	BOOST_REQUIRE(pAllocation != nullptr);

	auto end = lower.getMarker();
	BOOST_CHECK(end.position == begin.position + DEFAULT_STACK_MEM_SIZE);

	pStack->deallocateLower(begin);
	auto marker = lower.getMarker();
	BOOST_CHECK(marker.position == begin.position);
	BOOST_CHECK(marker.allocationIndex == begin.allocationIndex);
}

BOOST_AUTO_TEST_CASE(allocate_out_of_memory_in_middle)
{
	pStack->allocateLower(DEFAULT_STACK_MEM_SIZE / 2, 4);
	pStack->allocateUpper(DEFAULT_STACK_MEM_SIZE / 2, 4);

	BOOST_CHECK(pStack->getTotalNumAllocations() == 2);
	BOOST_CHECK(pStack->getTotalUsedMemory() == DEFAULT_STACK_MEM_SIZE);

	void* pAllocation = nullptr;
	BOOST_REQUIRE_THROW(pAllocation = pStack->allocateLower(4, 4), fs::assert_exception);
	BOOST_CHECK(pAllocation == nullptr);
}

BOOST_AUTO_TEST_CASE(get_totals)
{
	u32 allocationSize = DEFAULT_STACK_MEM_SIZE / 2;

	BOOST_CHECK(pStack->getTotalNumAllocations() == 0);
	BOOST_CHECK(pStack->getTotalUsedMemory() == 0);

	pStack->allocateLower(allocationSize, 4);
	BOOST_CHECK(pStack->getTotalNumAllocations() == 1);
	BOOST_CHECK(pStack->getTotalUsedMemory() == allocationSize);

	pStack->allocateUpper(allocationSize, 4);
	BOOST_CHECK(pStack->getTotalNumAllocations() == 2);
	BOOST_CHECK(pStack->getTotalUsedMemory() == allocationSize * 2);

	pStack->clear();
	BOOST_CHECK(pStack->getTotalNumAllocations() == 0);
	BOOST_CHECK(pStack->getTotalUsedMemory() == 0);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()