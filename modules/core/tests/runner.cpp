#define BOOST_TEST_MODULE FissuraCore

#include <boost/test/unit_test.hpp>

#include "core/assert.h"

struct TestConfig
{
    TestConfig()
    {
        fs::setAbortOnAssert(true);
    }

    ~TestConfig()
    {

    }
};

BOOST_GLOBAL_FIXTURE(TestConfig);
