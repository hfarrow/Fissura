#include "fscore.h"
#include "fsgame/game_app.h"

using namespace fs;

GameApp* g_pApp;
PageAllocator* gpGeneralPage = nullptr;
HeapAllocator* gpGeneralHeap = nullptr;
HeapAllocator* gpDebugHeap = nullptr;

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
#endif

	gpGeneralPage = new PageAllocator(L"gpGeneralPage");
	gpGeneralHeap = new HeapAllocator(L"gpGeneralHeap", *gpGeneralPage);
	gpDebugHeap = new HeapAllocator(L"gpDebugHeap", *gpGeneralPage);

	// TODO: startup logger
	// TODO: g_pApp->initOption("PlayrOptions.xml", lpCmdLine);

	int retCode = 0;
	if(g_pApp->init())
	{
		g_pApp->run();
		g_pApp->shutdown();
		retCode = 0;
	}
	else
	{
		FS_ASSERT(!"Failed to init GameApp");
		retCode = 1;
	}

	// TODO: shutdown logger

	delete gpDebugHeap;
	delete gpGeneralHeap;
	delete gpGeneralPage;

	return retCode;
}

GameApp::GameApp()
{
	g_pApp = this;
	_isRunning = false;
	pWindow = nullptr;
}

GameApp::~GameApp()
{

}

bool GameApp::init()
{
	SDL_Init(SDL_INIT_EVERYTHING | SDL_INIT_NOPARACHUTE);

	pWindow = SDL_CreateWindow(getGameTitle(),
							   SDL_WINDOWPOS_UNDEFINED,
							   SDL_WINDOWPOS_UNDEFINED,
							   800,
							   600,
							   SDL_WINDOW_OPENGL);

	return onInit();
}

void GameApp::run()
{
	_isRunning = true;

	SDL_Event event;

	while(_isRunning)
	{
		while(SDL_PollEvent(&event))
		{
			onEvent(&event);
		}

		onUpdate(0);
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

void GameApp::shutdown()
{
	onShutdown();

	SDL_DestroyWindow(pWindow);
	SDL_Quit();
}