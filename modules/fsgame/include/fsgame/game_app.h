#ifndef FS_GAME_APP_H
#define FS_GAME_APP_H

#include <SDL.h>

#include "fscore.h"

namespace fs
{
	class GameAppRunner : Uncopyable
	{
	public:
		GameAppRunner();
		~GameAppRunner();

		int runGameApp();
	private:
		bool _isRunning;
	};

	class GameApp : Uncopyable
	{
	public:
		friend class GameAppRunner;

		GameApp();
		virtual ~GameApp();

		void exit();

		virtual const char* getGameTitle() const = 0;

	protected:
		virtual bool onInit() = 0;
		virtual void onShutdown() = 0;

		virtual void onEvent(SDL_Event* pEvent);
		virtual void onUpdate(f32 elapsedTime) = 0;

	private:
		bool init();
		void run();
		void shutdown();

		bool _isRunning;
		SDL_Window* pWindow;
	};
}

#endif
