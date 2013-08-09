// PlatformAbstractionSpike.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <platforminterface.h>

int _tmain(int argc, _TCHAR* argv[])
{
	PlatformInterface pi = PlatformInterface();
	pi.function1();
	pi.function2(1);
	pi.function3();

	return 0;
}

