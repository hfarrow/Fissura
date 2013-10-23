#include <exception>
#include <signal.h>
#include <memory.h>

#include <boost/test/execution_monitor.hpp> 
#include <boost/test/unit_test_monitor.hpp>
#include <boost/test/unit_test.hpp>

#include "core/assert.h"
#include "core/platforms.h"
#include "core/allocators/pool_allocator.h"
#include "core/allocators/heap_allocator.h"
#include "core/allocators/trace_allocator.h"
#include "core/types.h"

#define DEFAULT_MEM_SIZE  64 * 1024 // bytes
#define DEBUG_MEM_SIZE 1048576 // 1mb

using namespace fs;
HeapAllocator* gpDebugHeap = nullptr;

BOOST_AUTO_TEST_SUITE(core)
BOOST_AUTO_TEST_SUITE(allocation)

struct traceallocator_fixture
{
	traceallocator_fixture()
	{
		pAllocator = nullptr;
		pMemory = nullptr;
		pDebugMemory = nullptr;
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
		delete gpDebugHeap;
		delete[] (u8*)pDebugMemory;

		currentMemorySize = size;
		if(size > 0)
		{
			pDebugMemory = new u8[DEBUG_MEM_SIZE]; // 1mb
			gpDebugHeap = new HeapAllocator(nullptr, DEBUG_MEM_SIZE, pDebugMemory);
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
    auto& monitor = boost::unit_test::unit_test_monitor_t::instance();
    BOOST_TEST_MESSAGE("TraceAllocator assert expected below here.");
    BOOST_REQUIRE_THROW(monitor.execute([&proxy](){proxy.reportMemoryLeaks(); return 0;}), boost::execution_exception);
    proxy.deallocate(p);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
