#include <boost/thread.hpp>

#include "fscore.h"
#include "fsgame/app/game_app.h"
#include "fsgame/app/memory_policy.h"
#include "fsgame/app/system_interfaces.h"
#include "fsgame/app/game_context.h"

using namespace fs;

GameApp* gpApp = nullptr;

GameAppRunner::GameAppRunner()
	: _isRunning(false)
{

}

GameAppRunner::~GameAppRunner()
{

}

int GameAppRunner::runGameApp()
{
	FS_ASSERT_MSG(!_isRunning, "runGameApp can only be called once per program execution.");
	_isRunning = true;

    Clock::init();

	// TODO: gpApp->initOption("PlayerOptions.xml", lpCmdLine);

	int retCode = 0;
	if(gpApp->init())
	{
		gpApp->run();
		gpApp->shutdown();
	}
	else
	{
		FS_ASSERT(!"Failed to init GameApp");
		retCode = 1;
	}


	return retCode;
}

GameApp::GameApp() :
    _pMemoryPolicy(nullptr),
    _pBasePath(nullptr),
    _isRunning(false),
    _pWindow(nullptr),
    _processManager(ProcessManager()),
    _clock(0)
{
	gpApp = this;
    GameContext::GameApp = this;
}

GameApp::~GameApp()
{

}


IMemoryPolicy* GameApp::createMemoryPolicy()
{
    static u8 policyMemory[sizeof(StandardMemoryPolicy)];
    IMemoryPolicy* pMemoryPolicy = new(policyMemory) StandardMemoryPolicy();
    return pMemoryPolicy;
}

bool GameApp::init()
{
    if(!initMemory())
    {
        return false;
    }

	SDL_Init(SDL_INIT_EVERYTHING | SDL_INIT_NOPARACHUTE);

    if(!initPaths() ||
       !initLogger() ||
       !initConfig() ||
       !initWindow())
    {
        return false;
    }

	return onInit();
}

bool GameApp::initMemory()
{
    _pMemoryPolicy = createMemoryPolicy();
    if(!_pMemoryPolicy)
    {
        FS_FATAL("Failed to create memory policy.");
        return false;
    }

    _pMemoryPolicy->init();

    return true;
}

bool GameApp::initPaths()
{
    char* pBasePath = SDL_GetBasePath();
    if(!pBasePath)
    {
        FS_FATALF(boost::format("Failed to get base path. Error: %1%") % SDL_GetError());
        return false;
    }

    FS_INFOF(boost::format("Base path is %1%") % pBasePath);
    _pBasePath = pBasePath;
    return true;
}

bool GameApp::initLogger()
{
    FS_ASSERT(_pBasePath);

    std::string configPath(_pBasePath);
    configPath += "content/logger.xml";
    Logger::init(configPath.c_str());

    return true;
}

bool GameApp::initConfig()
{
    FS_INFO("GameApp::initConfig");
    FS_ASSERT(_pBasePath);

    return true;
}

bool GameApp::initWindow()
{
    FS_INFO("GameApp::initWindow");
	_pWindow = SDL_CreateWindow(getGameTitle(),
							   SDL_WINDOWPOS_UNDEFINED,
							   SDL_WINDOWPOS_UNDEFINED,
							   800,
							   600,
							   SDL_WINDOW_OPENGL);

    if(!_pWindow)
    {
        FS_FATALF(boost::format("Failed to create window. Error: %1%") % SDL_GetError());
        return false;
    }

    return true;
}

void GameApp::run()
{
    FS_INFO("GameApp::run");
	_isRunning = true;

	SDL_Event event;

    const f32 dt = 1.0f / 60.0f;
    f32 accumulator = 0;
    u64 currentTime = _clock.getTimeCycles();

	while(_isRunning)
	{
        u64 newTime = SDL_GetPerformanceCounter();
        f32 frameTime = (f32)(newTime - currentTime) / (f32)SDL_GetPerformanceFrequency();
        if(frameTime > 1.0f / 10.0f)
        {
            // Long frame or resumed from breakpoint
            frameTime = dt;
        }

        currentTime = newTime;
        accumulator += frameTime;

		while(SDL_PollEvent(&event))
		{
			onEvent(&event);
		}

        while(accumulator >= dt)
        {
            f32 dtScaled = _clock.update(dt);
            _processManager.updateProcesses(dtScaled);
            onUpdate(dtScaled);
            accumulator -= dt;
        }

        //f32 percentage = accumulator / dt;
        // render(percentage);

#ifdef _DEBUG
        if(Memory::getTracker())
        {
            Memory::getTracker()->update();
        }
#endif
	}
}

void GameApp::onEvent(SDL_Event* pEvent)
{
	if(pEvent->type == SDL_QUIT)
	{
		exit();
	}
}

void GameApp::exit()
{
    FS_INFO("GameApp::exit");
	_isRunning = false;
}

const char* GameApp::getBasePath() const
{
    return _pBasePath;
}

const Clock* GameApp::getClock() const
{
    return &_clock;
}

ProcessManager* GameApp::getProcessManager()
{
    return &_processManager;
}

void GameApp::shutdown()
{
    FS_INFO("GameApp::shutdown");
	onShutdown();

    _processManager.abortAllProcesses(true);

    if(Memory::getTracker())
    {
        FS_INFO("Memory Report after onShutDown():");
        MemoryTracker::Report report = Memory::getTracker()->generateReport();
        for(auto it = report.allocators->begin(); it != report.allocators->end(); ++it)
        {
            FS_INFO(boost::format("    Allocator[%1%] total=%2% peak=%3%")
                    % it->pAllocator->getName()
                    % it->usedMemory
                    % it->peakUsedMemory);
        }
    }

	SDL_DestroyWindow(_pWindow);
    Logger::destroy();
    SDL_free(_pBasePath);
	SDL_Quit();
    _pMemoryPolicy->destroy();
}
