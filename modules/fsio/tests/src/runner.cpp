#define BOOST_TEST_MODULE FissuraCore

#include <boost/test/unit_test.hpp>
#include "fscore.h"
#include "fsio.h"

using namespace fs;

class GlobalFixture
{
public:
    GlobalFixture()
    {
    }

    ~GlobalFixture()
    {
    }
};

BOOST_GLOBAL_FIXTURE(GlobalFixture);
