

#define BOOST_TEST_MODULE FissuraCore

#include <boost/test/unit_test.hpp>
#include "fscore.h"

using namespace fs;
HeapAllocator* gpFsDebugHeap;
HeapAllocator* gpFsMainHeap;

class GlobalFixture
{
public:
    GlobalFixture()
    {
        fs::setAbortOnAssert(true);
        gpFsDebugHeap = nullptr;
        gpFsMainHeap = nullptr;
    }

    ~GlobalFixture()
    {
    }
};

BOOST_GLOBAL_FIXTURE(GlobalFixture);
