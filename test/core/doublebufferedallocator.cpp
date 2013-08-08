#include <boost/test/unit_test.hpp>

#include <core/allocators/stackallocator.h>
#include <core/allocators/doublebufferedallocator.h>
#include <core/types.h>
#include <exception>

#define DEFAULT_STACK_MEM_SIZE  256 // bytes

using namespace fs;

BOOST_AUTO_TEST_SUITE(core)
BOOST_AUTO_TEST_SUITE(allocation)

struct doublebufferedallocator_fixture
{
	doublebufferedallocator_fixture()
	{
		pStackA = nullptr;
		pStackDataA = nullptr;
		pStackB = nullptr;
		pStackDataB = nullptr;
		pAllocator = nullptr;
		resizeStack(DEFAULT_STACK_MEM_SIZE);
	}

	~doublebufferedallocator_fixture()
	{
		pAllocator->clearCurrentBuffer();
		pAllocator->clearOtherBuffer();
		pStackA->clear();
		pStackB->clear();
		resizeStack(0);
	}

	void resizeStack(u32 size)
	{
		delete pAllocator;
		delete pStackA;
		delete pStackDataA;
		delete pStackB;
		delete pStackDataB;

		currentStackSize = size;
		if(size > 0)
		{
			pStackDataA = new u8[size];
			pStackA = new StackAllocator(nullptr, size, pStackDataA);

			pStackDataB = new u8[size];
			pStackB = new StackAllocator(nullptr, size, pStackDataB);

			pAllocator = new DoubleBufferedAllocator(nullptr, *pStackA, *pStackB);
		}
	}

	u32 currentStackSize;
	StackAllocator* pStackA;
	StackAllocator* pStackB;
	DoubleBufferedAllocator* pAllocator;
	void* pStackDataA;
	void* pStackDataB;
};

BOOST_FIXTURE_TEST_SUITE(double_buffered_allocator, doublebufferedallocator_fixture)

BOOST_AUTO_TEST_CASE(swap_buffers)
{
	pAllocator->swapBuffers();
	pAllocator->allocate(DEFAULT_STACK_MEM_SIZE, 4);
	BOOST_CHECK(pAllocator->getStackB().getTotalNumAllocations() == 1);
}

BOOST_AUTO_TEST_CASE(clear_current_buffer)
{
	pAllocator->allocate(DEFAULT_STACK_MEM_SIZE, 4);
	pAllocator->swapBuffers();
	pAllocator->allocate(DEFAULT_STACK_MEM_SIZE, 4);

	pAllocator->swapBuffers();
	pAllocator->clearCurrentBuffer();
	BOOST_CHECK(pAllocator->getStackA().getTotalNumAllocations() == 0);
	BOOST_CHECK(pAllocator->getStackB().getTotalNumAllocations() == 1);

	pAllocator->swapBuffers();
	pAllocator->clearCurrentBuffer();
	BOOST_CHECK(pAllocator->getStackA().getTotalNumAllocations() == 0);
	BOOST_CHECK(pAllocator->getStackB().getTotalNumAllocations() == 0);
}

BOOST_AUTO_TEST_CASE(clear_other_buffer)
{
	pAllocator->allocate(DEFAULT_STACK_MEM_SIZE, 4);
	pAllocator->swapBuffers();
	pAllocator->allocate(DEFAULT_STACK_MEM_SIZE, 4);
	
	pAllocator->swapBuffers();
	pAllocator->clearOtherBuffer();
	BOOST_CHECK(pAllocator->getStackA().getTotalNumAllocations() == 1);
	BOOST_CHECK(pAllocator->getStackB().getTotalNumAllocations() == 0);

	pAllocator->swapBuffers();
	pAllocator->clearOtherBuffer();
	BOOST_CHECK(pAllocator->getStackA().getTotalNumAllocations() == 0);
	BOOST_CHECK(pAllocator->getStackB().getTotalNumAllocations() == 0);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()