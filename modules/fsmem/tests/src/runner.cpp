#define BOOST_TEST_MODULE fscore

#include <boost/test/unit_test.hpp>
#include "fscore.h"
#include "fsmem.h"

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
