#define BOOST_TEST_MODULE FissuraCore

#include <boost/test/unit_test.hpp>
#include "fscore.h"

using namespace fs;

class GlobalFixture
{
public:
    GlobalFixture()
    {
        // TODO: need a way to make asserts fail the unit tests because an automated
        // build would hang on the assert until it is skipped.
        // Maybe a function that would configure assert to crash on failure
        // ie: fs::setCrashOnAssert(true);
        //fs::setIgnoreAsserts(true);
    }

    ~GlobalFixture()
    {
    }
};

BOOST_GLOBAL_FIXTURE(GlobalFixture);
