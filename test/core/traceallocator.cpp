#include <boost/test/unit_test.hpp>

#include <core/poolallocator.h>
#include <core/heapallocator.h>
#include <core/traceallocator.h>
#include <core/types.h>
#include <exception>

#define DEFAULT_MEM_SIZE  64 * 1024 // bytes
#define DEBUG_MEM_SIZE 1048576 // 1mb

using namespace fissura;
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
		delete pMemory;
		delete gpDebugHeap;
		delete pDebugMemory;

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
	TraceAllocator proxy = TraceAllocator(nullptr, *pAllocator);
	void* p = proxy.allocate(8, 8);
	BOOST_REQUIRE(p != nullptr);
	proxy.deallocate(p);

	// Currently cannot test the stack trace of traceallocator becuase it is internal and does
	// not expose a public interface. This test case will simply ensure that the trace
	// allocator will not throw an exception or assert when allocating and deallocating.
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()