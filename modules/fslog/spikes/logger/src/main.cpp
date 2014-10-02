#include <iostream>
#include <map>
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "fscore.h"
#include "fsmem.h"
#include "fslog.h"

using namespace fs;
using namespace std;

int main( int, char **)
{
    Logger root("content/root.xml", "root");
    log::setRootLogger(&root);

    Logger child("content/child.xml", "child");
    root.addChild(&child);

    child.log("INFO", "Child Logged this", __FUNCTION__, __FILE__, __LINE__);


    FS_LOGF("INFO", "This is a test: %1%", "testing 123");
    // FS_FATALF("FATAL %1%", 1);
    // FS_ERRORF("ERROR %1%", 1);
    // FS_LOG("INFO", "LOG");
    // FS_FATAL("FATAL!");
    // FS_ERROR("ERROR!");
    // FS_WARN("WARN!");
    // FS_INFO("INFO!");
    // FS_DEBUG("DEBUG!");
    // FS_WARNF("WARN %1%", 1);
    // FS_INFOF("INFO %1%", 1);
    // FS_DEBUGF("DEBUG %1%", 1);

    // abort();

    return 0;
}
