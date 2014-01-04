#include <iostream>
#include <map>
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "fscore.h"

using namespace fs;
using namespace std;

Allocator* gpFsDebugHeap = nullptr;
Allocator* gpFsMainHeap = nullptr;

int main( int, char **)
{
    const u32 mainHeapSize = 1048576;
    u8 mainHeapMemory[mainHeapSize];
    u8 debugHeapMemory[mainHeapSize];

    HeapAllocator* pHeap = new HeapAllocator(L"gpFsMainHeap", mainHeapSize, (void*)mainHeapMemory);
    gpFsDebugHeap = new HeapAllocator(L"gpFsMainHeap", mainHeapSize, (void*)debugHeapMemory);
    Memory::setDefaultDebugAllocator(gpFsDebugHeap);
    gpFsMainHeap = new TraceAllocator(L"gpFsMainHeapTrace", *pHeap);
    Memory::setDefaultAllocator(gpFsMainHeap);

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
    
    Logger::destroy();
    
    gpFsMainHeap->clear();
    delete gpFsMainHeap;

    gpFsDebugHeap->clear();
    delete gpFsDebugHeap;

    delete pHeap;

    return 0;
}
