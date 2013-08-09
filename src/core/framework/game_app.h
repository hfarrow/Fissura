#ifndef FS_GAME_APP_H
#define FS_GAME_APP_H

#include "core/platforms.h"
#include "core/types.h"
#include "core/assert.h"

namespace fs
{
namespace internal
{
	template<u32 PlatformID>
	class GameApp
	{
	public:
		GameApp(const fschar* const pName){}
		virtual ~GameApp(){}

		void run();

	protected:

	private:

	};

	template<u32 PlatformID>
	void GameApp<PlatformID>::run()
	{
		FS_ASSERT("A platform specialization for GameApp must be provided for the current platform.");
	}
}
	typedef internal::GameApp<PLATFORM_ID> GameApp;
}

#endif