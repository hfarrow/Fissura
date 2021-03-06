
#define BOOST_TEST_MODULE FissuraGame

#include <boost/test/unit_test.hpp>
#include "fscore.h"

using namespace fs;
Allocator* gpFsDebugHeap;
Allocator* gpFsMainHeap;

class GlobalFixture
{
public:
    GlobalFixture()
    {
        fs::setIgnoreAsserts(true);
        gpFsDebugHeap = nullptr;
        gpFsMainHeap = nullptr;
    }

    ~GlobalFixture()
    {
    }
};

BOOST_GLOBAL_FIXTURE(GlobalFixture);
