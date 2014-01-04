#include <exception>
#include <signal.h>
#include <memory.h>

#include <boost/test/unit_test.hpp>

#include "fscore.h"
#include "fstest.h"

#define DEFAULT_MEM_SIZE  64 * 1024 // bytes
#define DEBUG_MEM_SIZE 1048576 // 1mb

using namespace fs;

BOOST_AUTO_TEST_SUITE(core)
BOOST_AUTO_TEST_SUITE(allocators)

struct traceallocator_fixture
{
	traceallocator_fixture()
	{
		pAllocator = nullptr;
		pMemory = nullptr;
		resizeMemory(DEFAULT_MEM_SIZE);
	}

	~traceallocator_fixture()
	{
		resizeMemory(0);
	}

	void resizeMemory(u32 size)
	{
		delete pAllocator;
		delete[] (u8*)pMemory;

		currentMemorySize = size;
        Memory::getDefaultDebugAllocator()->clear();
        Memory::getDefaultAllocator()->clear();
		if(size > 0)
		{
			pMemory = new u8[size];
			pAllocator = new PoolAllocator(nullptr, 8, 8, size, pMemory);
		}
	}

	size_t currentMemorySize;
	PoolAllocator* pAllocator;
	void* pMemory;
	void* pDebugMemory;
};

BOOST_FIXTURE_TEST_SUITE(trace_allocator, traceallocator_fixture)

BOOST_AUTO_TEST_CASE(record_stack_trace)
{
	TraceAllocator proxy(nullptr, *pAllocator);
	void* p = proxy.allocate(8, 8);
	BOOST_REQUIRE(p != nullptr);
	proxy.deallocate(p);

    p = proxy.allocate(8,8);
    FS_REQUIRE_ASSERT([&proxy](){proxy.reportMemoryLeaks();});
    proxy.deallocate(p);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
