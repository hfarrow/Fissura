#include <boost/test/unit_test.hpp>

#include <core/pageallocator.h>
#include <core/types.h>
#include <exception>

#define DEFAULT_MEM_SIZE  64 * 1024 // bytes

using namespace fissura;

BOOST_AUTO_TEST_SUITE(core)
BOOST_AUTO_TEST_SUITE(allocation)

struct pageallocator_fixture
{
	pageallocator_fixture()
	{
		pAllocator = nullptr;
		pMemory = nullptr;
		resizeMemory(DEFAULT_MEM_SIZE);
	}

	~pageallocator_fixture()
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
			pAllocator = new PageAllocator(nullptr, 4096 * 3 + 10);
		}
	}

	size_t currentMemorySize;
	PageAllocator* pAllocator;
	void* pMemory;
};

BOOST_FIXTURE_TEST_SUITE(pageallocator, pageallocator_fixture)

BOOST_AUTO_TEST_CASE(allocate_and_deallocate)
{
	
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()