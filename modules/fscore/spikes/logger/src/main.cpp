#include <iostream>
#include <map>
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "fscore.h"

using namespace fs;
using namespace std;

int main( int, char **)
{
    Logger::init("content/logger.xml");

    FS_LOGF("INFO", "This is a test: %1%", "testing 123");
    FS_FATALF("FATAL %1%", 1);
    FS_ERRORF("ERROR %1%", 1);
    FS_LOG("INFO", "LOG");
    FS_FATAL("FATAL!");
    FS_ERROR("ERROR!");
    FS_WARN("WARN!");
    FS_INFO("INFO!");
    FS_DEBUG("DEBUG!");
    FS_WARNF("WARN %1%", 1);
    FS_INFOF("INFO %1%", 1);
    FS_DEBUGF("DEBUG %1%", 1);

    // FS_DEBUG("TESTING FS_DEBUG");
    // FS_INFO("TESTING FS_INFO");
    // FS_WARN("TESTING FS_WARN");
    // FS_ERROR("TESTING FS_ERROR");
    // FS_FATAL("TESTING FS_FATAL");
    // FS_DEBUGF(dformat("TESTING FS_DEBUGF"));
    // FS_INFOF(dformat("TESTING FS_INFOF %1%") % 0.4f );
    // FS_WARNF(dformat("TESTING FS_WARNF %d %s") % 0 % "testArgInt");
    // FS_ERRORF(dformat("TESTING FS_ERRORF %2% %1%") % "testArg1" % "testArg2");
    // FS_FATALF(dformat("TESTING FS_FATALF %2% %1%") % "testArg1" % "testArg2");

    // abort();

    Logger::destroy();

    return 0;
}
