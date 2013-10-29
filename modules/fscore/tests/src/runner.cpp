

#define BOOST_TEST_MODULE FissuraCore

#include <boost/test/unit_test.hpp>
#include "fscore.h"

using namespace fs;
HeapAllocator* gpDebugHeap;

class GlobalFixture
{
public:
    GlobalFixture()
    {
        fs::setAbortOnAssert(true);
        gpDebugHeap = nullptr;
    }

    ~GlobalFixture()
    {
    }
};

BOOST_GLOBAL_FIXTURE(GlobalFixture);
