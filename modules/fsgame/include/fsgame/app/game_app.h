#ifndef FS_GAME_APP_H
#define FS_GAME_APP_H

#include <SDL.h>

#include "fscore.h"

namespace fs
{
    class IMemoryPolicy;

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
        const Clock& getClock() const;

		virtual const char* getGameTitle() const = 0;
        const char* getBasePath() const;

	protected:
		virtual bool onInit() = 0;
		virtual void onShutdown() = 0;

		virtual void onEvent(SDL_Event* pEvent);
		virtual void onUpdate(f32 elapsedTime) = 0;
        
        virtual IMemoryPolicy* createMemoryPolicy(); 

	private:
		bool init();
        bool initMemory();
        bool initPaths();
        bool initLogger();
        bool initConfig();
        bool initWindow();
		void run();
		void shutdown();

        IMemoryPolicy* _pMemoryPolicy;
        char* _pBasePath;
		bool _isRunning;
		SDL_Window* _pWindow;
        Clock _clock;
	};
}

#endif
