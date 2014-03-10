#define BOOST_TEST_MODULE FissuraCore

#include <boost/test/unit_test.hpp>
#include "fscore.h"

using namespace fs;

class GlobalFixture
{
public:
    GlobalFixture()
    {
        fs::setIgnoreAsserts(true);
    }

    ~GlobalFixture()
    {
    }
};

BOOST_GLOBAL_FIXTURE(GlobalFixture);
