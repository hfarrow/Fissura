#include <boost/thread.hpp>

#include "fsgame/game_app.h"
#include "fscore.h"

using namespace fs;

GameApp* gpApp = nullptr;
PageAllocator* gpGeneralPage = nullptr;
Allocator* gpFsMainHeap = nullptr;
#ifdef _DEBUG
Allocator* gpFsDebugHeap = nullptr;
#endif

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

	// Set up checks for memory leaks.
#ifdef WINDOWS
//#ifdef _DEBUG
	int tmpDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);

	// set this flag to keep memory blocks around
	// tmpDbgFlag |= _CRTDBG_DELAY_FREE_MEM_DF;	       // this flag will cause intermittent pauses in your game and potientially cause it to run out of memory!

	// perform memory check for each alloc/dealloc
	//tmpDbgFlag |= _CRTDBG_CHECK_ALWAYS_DF;		   // remember this is VERY VERY SLOW!

	//_CRTDBG_LEAK_CHECK_DF is used at program initialization to force a 
	//   leak check just before program exit. This is important because
	//   some classes may dynamically allocate memory in globally constructed
	//   objects.
	tmpDbgFlag |= _CRTDBG_LEAK_CHECK_DF;					

	_CrtSetDbgFlag(tmpDbgFlag);
//#endif
#endif


    u8 pAllocatorMemory[sizeof(PageAllocator) + (2 * sizeof(HeapAllocator))];
    u8* pNextAllocation = pAllocatorMemory;
	gpGeneralPage = new(pNextAllocation) PageAllocator(L"gpGeneralPage");
#ifdef _DEBUG
    pNextAllocation += u8(sizeof(PageAllocator));
	gpFsDebugHeap = new(pNextAllocation) HeapAllocator(L"gpFsDebugHeap", *gpGeneralPage);
    Memory::setDefaultDebugAllocator(gpFsDebugHeap);
#endif
    pNextAllocation += u8(sizeof(HeapAllocator));
	gpFsMainHeap = new(pNextAllocation) HeapAllocator(L"gpFsMainHeap", *gpGeneralPage);
    Memory::setDefaultAllocator(gpFsMainHeap);
    
#ifdef _DEBUG
    Memory::initTracker();
    MemoryTracker* pTracker = Memory::getTracker();
    FS_ASSERT(pTracker);
    // Normally allocators automatically register themselves but these
    // allocators must be created before the tracker is initialized
    // because the tracker init needs to allocate memory from debug heap.
    pTracker->registerAllocator(gpGeneralPage);
    pTracker->registerAllocator(gpFsDebugHeap);
    pTracker->registerAllocator(gpFsMainHeap);
#endif


    Clock::init();
    Logger::init("logger.xml");

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

    Logger::destroy();
    
#ifdef _DEBUG
    Memory::destroyTracker();
#endif

	static_cast<HeapAllocator*>(gpFsMainHeap)->~HeapAllocator();
#ifdef _DEBUG
	static_cast<HeapAllocator*>(gpFsDebugHeap)->~HeapAllocator();
#endif
	gpGeneralPage->~PageAllocator();

	return retCode;
}

GameApp::GameApp() :
    _clock(0)
{
	gpApp = this;
	_isRunning = false;
	pWindow = nullptr;
}

GameApp::~GameApp()
{

}

bool GameApp::init()
{
	SDL_Init(SDL_INIT_EVERYTHING | SDL_INIT_NOPARACHUTE);
    
    if(!initConfig())
        return false;

    if(!initWindow())
        return false;
    
	return onInit();
}

bool GameApp::initConfig()
{

    return true;
}

bool GameApp::initWindow()
{
	pWindow = SDL_CreateWindow(getGameTitle(),
							   SDL_WINDOWPOS_UNDEFINED,
							   SDL_WINDOWPOS_UNDEFINED,
							   800,
							   600,
							   SDL_WINDOW_OPENGL);

    if(!pWindow)
    {
        FS_ASSERT_MSG_FORMATTED(false, boost::format("Failed to create window. Error: %s") % SDL_GetError());
        return false;
    }

    return true;
}

void GameApp::run()
{
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
            onUpdate(dtScaled);
            accumulator -= dt;
        }

        //f32 percentage = accumulator / dt;
        // render(percentage);
        
#ifdef _DEBUG
        Memory::getTracker()->update();
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
	_isRunning = false;
}

const Clock& GameApp::getClock() const
{
    return _clock;
}

void GameApp::shutdown()
{
	onShutdown();

	SDL_DestroyWindow(pWindow);
	SDL_Quit();
}
