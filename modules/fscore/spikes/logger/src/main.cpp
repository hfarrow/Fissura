#include <iostream>
#include <map>
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>

#include "fscore.h"

using namespace fs;

HeapAllocator* gpFsDebugHeap = nullptr;
HeapAllocator* gpFsMainHeap = nullptr;

int main( int, char **)
{
    const u32 mainHeapSize = 1048576;
    u8 mainHeapMemory[mainHeapSize];
    u8 debugHeapMemory[mainHeapSize];
    gpFsMainHeap = new HeapAllocator(L"gpFsMainHeap", mainHeapSize, (void*)mainHeapMemory);
    gpFsDebugHeap = new HeapAllocator(L"gpFsMainHeap", mainHeapSize, (void*)debugHeapMemory);

    Logger::init("fscore-logger-content/logger.xml");
    FS_DEBUG("TESTING FS_DEBUG");
    FS_INFO("TESTING FS_INFO");
    FS_WARN("TESTING FS_WARN");
    FS_ERROR("TESTING FS_ERROR");
    FS_FATAL("TESTING FS_FATAL");
    FS_DEBUGF(boost::format("TESTING FS_DEBUGF"));
    FS_INFOF(boost::format("TESTING FS_INFOF %1%") % 0.4f );
    FS_WARNF(boost::format("TESTING FS_WARNF %d %s") % 0 % "testArgInt");
    FS_ERRORF(boost::format("TESTING FS_ERRORF %2% %1%") % "testArg1" % "testArg2");
    FS_FATALF(boost::format("TESTING FS_FATALF"));
    
    gpFsDebugHeap->clear();
    gpFsMainHeap->clear();

    delete gpFsDebugHeap;
    delete gpFsMainHeap;

    return 0;
}
