#include "stdafx.h"
#include "platforminterface.h"

void PlatformInterface::function1()
{
	FS_TRACE("WINDOWS function1()");
}

int PlatformInterface::function2(int arg)
{
	FS_TRACE_FORMATTED("WINDOWS function2(%i)", arg);
	return arg;
}


void PlatformInterface::function3()
{
	FS_TRACE("WINDOWS function3()");
	BasePlatformInterface::function3();
}