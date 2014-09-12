#include <iostream>
#include <map>
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <cstdlib>

#include "fscore.h"

using namespace std;
using namespace chrono;

using namespace fs;

steady_clock::time_point gStartTime;

void startBenchmark()
{
    gStartTime = steady_clock::now();
}

duration<double, milli> endBenchmark()
{
    return duration<double, milli>(steady_clock::now() - gStartTime);
}

int main( int, char **)
{
    //Logger::init("content/logger.xml");

    int x = 0;
    for(int xx = 0; xx < 5; ++xx)
    {
        startBenchmark();
        {
            std::function<int(int)> t2 = [&x](int i){ return i + x; };
            std::function<void(int)> t1 = [&x, &t2](int i){ x = t2(i); };
            for(int i = 0; i < 1000000000; ++i) t1(i);
        }
        FS_PRINT("std::func: " << endBenchmark().count());

        startBenchmark();
        {
            Delegate<int(int)> t2 = [&x](int i){ return i + x; };
            Delegate<void(int)> t1 = [&x, &t2](int i){ x = t2(i); };
            for(int i = 0; i < 1000000000; ++i) t1(i);
        }
        FS_PRINT("delegate:  " << endBenchmark().count());
    }

    //Logger::destroy();

    return 0;
}

