#include <exception>

#include <boost/test/unit_test.hpp>

#include "fstest.h"
#include "fscore.h"

#define DEFAULT_MEM_SIZE  64 * 1024 // bytes

using namespace fs;

BOOST_AUTO_TEST_SUITE(core)
BOOST_AUTO_TEST_SUITE(allocation)

struct proxyallocator_fixture
{
	proxyallocator_fixture()
	{
		pAllocator = nullptr;
		pMemory = nullptr;
		resizeMemory(DEFAULT_MEM_SIZE);
	}

	~proxyallocator_fixture()
	{
		resizeMemory(0);
	}

	void resizeMemory(u32 size)
	{
		delete pAllocator;
		delete[] (u8*)pMemory;

		currentMemorySize = size;
		if(size > 0)
		{
			pMemory = new u8[size];
			pAllocator = new PoolAllocator(nullptr, 8, 8, size, pMemory);
		}
	}

	size_t currentMemorySize;
	PoolAllocator* pAllocator;
	void* pMemory;
};

BOOST_FIXTURE_TEST_SUITE(proxy_allocator, proxyallocator_fixture)

BOOST_AUTO_TEST_CASE(allocate_and_deallocate)
{
	ProxyAllocator proxyA(L"ProxyA", *pAllocator);
	ProxyAllocator proxyB(L"ProxyB", *pAllocator);

	u64* pA1 = proxyA.allocateConstruct<u64>();
	BOOST_CHECK(pA1 != nullptr);
	BOOST_CHECK(proxyA.getTotalNumAllocations() == 1);
	BOOST_CHECK(proxyA.getTotalUsedMemory() == 8);
	BOOST_CHECK(proxyB.getTotalNumAllocations() == 0);
	BOOST_CHECK(proxyB.getTotalUsedMemory() == 0);

	u64* pB1 = proxyB.allocateConstruct<u64>();
	BOOST_CHECK(pB1 != nullptr);
	BOOST_CHECK(proxyA.getTotalNumAllocations() == 1);
	BOOST_CHECK(proxyA.getTotalUsedMemory() == 8);
	BOOST_CHECK(proxyB.getTotalNumAllocations() == 1);
	BOOST_CHECK(proxyB.getTotalUsedMemory() == 8);

	proxyB.deallocateDestruct(pB1);
	BOOST_CHECK(proxyA.getTotalNumAllocations() == 1);
	BOOST_CHECK(proxyA.getTotalUsedMemory() == 8);
	BOOST_CHECK(proxyB.getTotalNumAllocations() == 0);
	BOOST_CHECK(proxyB.getTotalUsedMemory() == 0);

	proxyA.deallocateDestruct(pA1);
	BOOST_CHECK(proxyA.getTotalNumAllocations() == 0);
	BOOST_CHECK(proxyA.getTotalUsedMemory() == 0);
	BOOST_CHECK(proxyB.getTotalNumAllocations() == 0);
	BOOST_CHECK(proxyB.getTotalUsedMemory() == 0);
}

BOOST_AUTO_TEST_CASE(excede_memory_budget)
{
	ProxyAllocator proxy(L"Proxy", *pAllocator, 8);
	proxy.allocate(8, 8);

	FS_REQUIRE_ASSERT([&](){proxy.allocate(8, 8);});
	proxy.clear();
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()