#include "stdafx.h"
#include <core/assert.h>
#include <core/types.h>
#include <core/trace.h>

using namespace fissura;

const u32 gDebugStrBufferSize = 2048;

bool reportAssertFailure(char* const condition,
						 char* const strFile,
						 u32 nLine,
						 char* const format, ...)
{
	static char userMessage[gDebugStrBufferSize];
	static char output[gDebugStrBufferSize];
	va_list arg;
	va_start(arg, format);

	vsprintf_s(userMessage, format, arg);
	sprintf_s(output, "ASSERT FAILED: ( %s ) Line %i in %s -> %s", condition, nLine, strFile, userMessage);
	FS_TRACE_WARN(output);

	sprintf_s(output,
		"ASSERT FAILED: ( %s )\n\n Message: %s\n\n Line %i in %s\n\nClick OK to break to the debugger.", 
		condition, userMessage, nLine, strFile);

	va_end(arg);

#ifndef FS_SURPRESS_ASSERTS
	int result = MessageBoxA(0,output, "ASSERTION ERROR", MB_OKCANCEL);
	switch (result)
	{
		case IDOK:
			return true;
	}
#endif
	return false;
}