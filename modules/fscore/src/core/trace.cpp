#include <iostream>
#include <stdio.h>

#include "fscore/trace.h"
#include "fscore/types.h"

namespace fs
{
    bool tracesEnabled = true;
}

using namespace fs;

const u32 gDebugStrBufferSize = 4096;

void fs::traceDebugStringFormatted(const char* category, const char* format, ...)
{
	static char userMessage[gDebugStrBufferSize];
	static char output[gDebugStrBufferSize];

	va_list arg;
	va_start(arg, format);

	vsprintf(userMessage, format, arg);
	sprintf(output, "[%s] %s\n", category, userMessage);

    if(fs::tracesEnabled)
    {
#ifdef WIN32
        OutputDebugStringA(output);
#endif
        std::cout << output;
    }

	va_end(arg);
}
