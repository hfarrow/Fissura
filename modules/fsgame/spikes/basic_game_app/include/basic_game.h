#ifndef FS_BASIC_GAME_H
#define FS_BASIC_GAME_H

#include "fscore.h"
#include "fsgame.h"

namespace fs
{
    class Game : public GameApp
    {
    public:
        Game()
        {
        
        }

        virtual ~Game()
        {
        
        }

        virtual const char* getGameTitle() const override
        {
            return "BasicGameApp";
        }

    protected:
        virtual bool onInit() override
        {
            return true;
        }

        virtual void onShutdown() override
        {
            MemoryTracker::Report report = Memory::getTracker()->generateReport();
            for(auto it = report.allocators->begin(); it != report.allocators->end(); ++it)
            {
                FS_INFO(boost::format("Allocator[%1%] total=%2% peak=%3%")
                        % it->pAllocator->getName() 
                        % it->usedMemory
                        % it->peakUsedMemory);
            }
        }

        virtual void onEvent(SDL_Event* pEvent) override
        {
            GameApp::onEvent(pEvent);
        }

        virtual void onUpdate(f32 elapsedTime) override
        {
        
        }

    private:
    };
}

#endif
