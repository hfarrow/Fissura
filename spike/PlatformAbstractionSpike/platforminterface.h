#ifndef PLATFORM_INTERFACE_H
#define PLATFORM_INTERFACE_H

#define PLATFORM_WINDOWS	1
#define PLATFORM_LINUX		2
#define PLATFORM_MACOS		3
#define PLATFORM_ANDROID	4
#define PLATFORM_IOS		5

#if defined(_WIN32)
	#define PLATFORM_ID		PLATFORM_WINDOWS
#elif defined(__ANDROID__)		// must come before __linux__ as Android also #defines __linux__
	#define PLATFORM_ID		PLATFORM_ANDROID
#elif defined(__linux__)
	#define PLATFORM_ID		PLATFORM_LINUX
#elif defined(__MACH__)
	#include <TargetConditionals.h>
	#if (TARGET_OS_IPHONE == 1)
		#define PLATFORM_ID		PLATFORM_IOS
	#else
		#define PLATFORM_ID		PLATFORM_MACOS
	#endif
#endif

#include <core/trace.h>

namespace internal
{
	class BasePlatformInterface
	{
	public:
		virtual void function1() = 0;
		virtual int function2(int arg) = 0;

		virtual void function3()
		{
			FS_TRACE("BasePlatformInterface function3()");
		}
	};

	template<int PlatformID>
	class PlatformInterface : public BasePlatformInterface
	{
	public:
		virtual void function1() override;
		virtual int function2(int arg) override;
		//virtual void function3() override;
	};

	template<>
	class PlatformInterface<PLATFORM_WINDOWS> : public BasePlatformInterface
	{
	public:
		virtual void function1() override;
		virtual int function2(int arg) override;
		virtual void function3() override;
	};

	#include "platforminterface.inl"
}

typedef internal::PlatformInterface<PLATFORM_ID> PlatformInterface;

#endif