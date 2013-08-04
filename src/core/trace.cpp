#include "stdafx.h"
#include <core/trace.h>
#include <iostream>

using namespace fissura;

const u32 gDebugStrBufferSize = 2048;

void fissura::traceDebugStringFormatted(char* category, const char* format, ...)
{
	static char userMessage[gDebugStrBufferSize];
	static char output[gDebugStrBufferSize];

	va_list arg;
	va_start(arg, format);

	vsprintf_s(userMessage, format, arg);
	sprintf_s(output, "[%s] %s\n", category, userMessage);
	OutputDebugStringA(output);
	std::cout << output;

	va_end(arg);
}