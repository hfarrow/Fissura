#include <cstring>

#include <boost/test/unit_test.hpp>

#include "fstest.h"
#include "fscore.h"

#define DEFAULT_MEM_SIZE 1048576 // 1mb

using namespace fs;

BOOST_AUTO_TEST_SUITE(core)

struct string_fixture
{
	string_fixture()
	{
		pDebugMemory = nullptr;
        pMainMemory = nullptr;
        gpFsDebugHeap = nullptr;
        gpFsMainHeap = nullptr;
		resizeMemory(DEFAULT_MEM_SIZE);
	}

	~string_fixture()
	{
		resizeMemory(0);
	}

	void resizeMemory(u32 size)
	{
		delete gpFsDebugHeap;
        delete gpFsMainHeap;
        gpFsDebugHeap = nullptr;
        gpFsMainHeap = nullptr;
		delete[] (u8*)pDebugMemory;
		delete[] (u8*)pMainMemory;

		currentMemorySize = size;
		if(size > 0)
		{
			pDebugMemory = new u8[size]; // 1mb
			pMainMemory = new u8[size]; // 1mb
			gpFsDebugHeap = new HeapAllocator(nullptr, size, pDebugMemory);
			gpFsMainHeap = new HeapAllocator(nullptr, size, pMainMemory);
		}
	}

	size_t currentMemorySize;
	PageAllocator* pAllocator;
	void* pDebugMemory;
    void* pMainMemory;
};

BOOST_FIXTURE_TEST_SUITE(string, string_fixture)

// The main concern to test here was that StlAllocator for fs:string was
// allocating and deallocating memory as expected.
BOOST_AUTO_TEST_CASE(general_string_test)
{
    {
        BOOST_REQUIRE(gpFsMainHeap->getTotalNumAllocations() == 0);
        fs::string s = "Hello";
        BOOST_REQUIRE(gpFsMainHeap->getTotalNumAllocations() > 0);

        s += " World";
        BOOST_CHECK(strcmp(s.c_str(), "Hello World") == 0);
        BOOST_CHECK(s.compare(fs::string("Hello World")) == 0);
        BOOST_CHECK(s.compare(fs::string("Wrong")) != 0);
    }

    // Ensure strings were deallocated when they left scope.
    BOOST_REQUIRE(gpFsMainHeap->getTotalNumAllocations() == 0);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
