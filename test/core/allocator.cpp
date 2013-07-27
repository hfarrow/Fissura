#include <boost/test/unit_test.hpp>

#include <core/stackallocator.h>
#include <core/allocator.h>
#include <core/types.h>
#include <exception>

#define DEFAULT_STACK_MEM_SIZE  256 // bytes

using namespace fissura;

BOOST_AUTO_TEST_SUITE(core)
BOOST_AUTO_TEST_SUITE(allocation)

struct allocator_fixture
{
	allocator_fixture()
	{
		pStackData = nullptr;
		pAllocator = nullptr;
		resizeStack(DEFAULT_STACK_MEM_SIZE);
	}

	~allocator_fixture()
	{
		static_cast<StackAllocator*>(pAllocator)->clear();
		resizeStack(0);
	}

	void resizeStack(u32 size)
	{
		delete pAllocator;
		delete pStackData;

		currentStackSize = size;
		if(size > 0)
		{
			pStackData = new u8[size];
			pAllocator = new StackAllocator(size, pStackData);
		}
	}

	u32 currentStackSize;
	void* pStackData;
	Allocator* pAllocator;
};

struct TestObject
{
	u32 a, b, c, d;
};

BOOST_FIXTURE_TEST_SUITE(allocator, allocator_fixture)

BOOST_AUTO_TEST_CASE(allocate_deallocate_object)
{
	TestObject* pObj = pAllocator->allocateNew<TestObject>();
	BOOST_REQUIRE(pObj != nullptr);

	// StackAllocator cannot deallocate. This test will need to use a different
	// type of allocator once one has been created.
	//pAllocator->deallocateDelete<TestObject>(pObj);
	//BOOST_CHECK(pAllocator->getAllocator().getTotalNumAllocations() == 0);
}

BOOST_AUTO_TEST_CASE(allocate_deallocate_array)
{
	TestObject* pArray = pAllocator->allocateArray<TestObject>(3);
	BOOST_REQUIRE(pArray != nullptr);

	// + sizeof(TestObject) for array header
	BOOST_CHECK(pAllocator->getTotalUsedMemory() == sizeof(TestObject) * 3 + sizeof(TestObject));
	BOOST_CHECK(pAllocator->getTotalNumAllocations() == 1);

	// StackAllocator cannot deallocate. This test will need to use a different
	// type of allocator once one has been created.
	//pAllocator->deallocateArray<TestObject>(pArray);
	//BOOST_CHECK(pAllocator->getAllocator().getTotalUsedMemory() == 0);
	//BOOST_CHECK(pAllocator->getAllocator().getTotalNumAllocations() == 0);
}

BOOST_AUTO_TEST_CASE(calc_array_header_size)
{
	u32 intHeader = calcArrayHeaderSize<u32>();
	BOOST_CHECK(intHeader == 1);

	u32 testObjectHeader = calcArrayHeaderSize<TestObject>();
	BOOST_CHECK(testObjectHeader == 1);

	u32 shortHeader = calcArrayHeaderSize<u16>();
	BOOST_CHECK(shortHeader == 2);

	u32 charHeader = calcArrayHeaderSize<u8>();
	BOOST_CHECK(charHeader == 4);
}

BOOST_AUTO_TEST_CASE(next_aligned_address)
{
	BOOST_CHECK((uptr)0 == (uptr)nextAlignedAddress((void*)0, 2));
	BOOST_CHECK((uptr)2 == (uptr)nextAlignedAddress((void*)1, 2));
	BOOST_CHECK((uptr)0 == (uptr)nextAlignedAddress((void*)0, 4));
	BOOST_CHECK((uptr)4 == (uptr)nextAlignedAddress((void*)2, 4));
	BOOST_CHECK((uptr)0 == (uptr)nextAlignedAddress((void*)0, 16));	
	BOOST_CHECK((uptr)16 == (uptr)nextAlignedAddress((void*)2, 16));
	BOOST_CHECK((uptr)16 == (uptr)nextAlignedAddress((void*)4, 16));
	BOOST_CHECK((uptr)16 == (uptr)nextAlignedAddress((void*)13, 16));
	BOOST_CHECK((uptr)16 == (uptr)nextAlignedAddress((void*)16, 16));
}

BOOST_AUTO_TEST_CASE(align_adjustment)
{
	BOOST_CHECK( 0 == alignAdjustment((void*)0, 2));
	BOOST_CHECK( 1 == alignAdjustment((void*)1, 2));
	BOOST_CHECK( 0 == alignAdjustment((void*)0, 4));
	BOOST_CHECK( 3 == alignAdjustment((void*)1, 4));
	BOOST_CHECK( 2 == alignAdjustment((void*)2, 4));
	BOOST_CHECK( 1 == alignAdjustment((void*)3, 4));
	BOOST_CHECK( 15 == alignAdjustment((void*)1, 16));
	BOOST_CHECK( 3 == alignAdjustment((void*)13, 16));
}

BOOST_AUTO_TEST_CASE(align_adjustment_with_header)
{
	BOOST_CHECK( 4 == alignAdjustmentWithHeader((void*)0, 2, 4));
	BOOST_CHECK( 5 == alignAdjustmentWithHeader((void*)1, 2, 4));
	BOOST_CHECK( 4 == alignAdjustmentWithHeader((void*)0, 4, 4));
	BOOST_CHECK( 7 == alignAdjustmentWithHeader((void*)1, 4, 4));
	BOOST_CHECK( 6 == alignAdjustmentWithHeader((void*)2, 4, 4));
	BOOST_CHECK( 5 == alignAdjustmentWithHeader((void*)3, 4, 4));
	BOOST_CHECK( 16 == alignAdjustmentWithHeader((void*)0, 16, 4));
	BOOST_CHECK( 14 == alignAdjustmentWithHeader((void*)2, 16, 4));
	BOOST_CHECK( 7 == alignAdjustmentWithHeader((void*)9, 16, 4));
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()