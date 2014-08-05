#ifndef FS_GAME_APP_H
#define FS_GAME_APP_H

#include <SDL.h>

#include "fscore.h"
#include "fsgame/app/system_interfaces.h"
#include "fsgame/process/process_manager.h"

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

	class GameApp : public IGameApp, Uncopyable
	{
	public:
		friend class GameAppRunner;

		GameApp();
		virtual ~GameApp();

        virtual const char* getBasePath() const override;
        virtual ProcessManager* getProcessManager() override;
        virtual const Clock* getClock() const override;
		virtual void exit() override;


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
        ProcessManager _processManager;
        Clock _clock;
	};
}

#endif
