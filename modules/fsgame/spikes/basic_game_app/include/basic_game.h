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
            StlAllocator<DelayProcess> allocator(*Memory::getDefaultSystemAllocator());
            StrongProcessPtr pDelay = std::allocate_shared<DelayProcess>(allocator, 3);
            getProcessManager()->attachProcess(pDelay);

            return true;
        }

        virtual void onShutdown() override
        {
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
