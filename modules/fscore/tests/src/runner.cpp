#define BOOST_TEST_MODULE fscore

#include <boost/test/unit_test.hpp>
#include "fscore.h"

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
