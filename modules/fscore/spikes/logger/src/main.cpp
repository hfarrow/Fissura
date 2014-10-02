#include <iostream>
#include "fscore/log.h"
#include "fscore/assert.h"
#include "fscore/types.h"

using namespace fs;


int main( int, char **)
{
    ILogger* pLogger = core::getLogger();

    std::cout << (void*)pLogger << std::endl;

    pLogger->log("INFO", __FUNCTION__, __FILE__, __LINE__, "Test: %s", "Testing 1,2,3");

    FS_ASSERT_MSG(false, "testing assert");
    FS_ASSERT_MSG_FORMATTED(false, "Formatted Assert: %s", "arg1");

    return 0;
}

