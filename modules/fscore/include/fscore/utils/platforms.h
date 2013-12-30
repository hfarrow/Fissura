#ifndef FS_PLATFORMS_H
#define FS_PLATFORMS_H

#define PLATFORM_WINDOWS	1
#define PLATFORM_LINUX		2
#define PLATFORM_MACOS		3
#define PLATFORM_ANDROID	4
#define PLATFORM_IOS		5

#if defined(_WIN32)
	#define PLATFORM_ID		PLATFORM_WINDOWS
    #define WINDOWS
#elif defined(__ANDROID__)		// must come before __linux__ as Android also #defines __linux__
	#define PLATFORM_ID		PLATFORM_ANDROID
    #define ANDROID
#elif defined(__linux__)
	#define PLATFORM_ID		PLATFORM_LINUX
    #define LINUX
#elif defined(__MACH__)
	#include <TargetConditionals.h>
	#if (TARGET_OS_IPHONE == 1)
		#define PLATFORM_ID		PLATFORM_IOS
        #define IOS
	#else
		#define PLATFORM_ID		PLATFORM_MACOS
        #define MACOS
	#endif
#endif

#endif
