#ifndef FS_GAME_CONTEXT_H
#define FS_GAME_CONTEXT_H

#include "fscore.h"
#include "fsgame/app/system_interfaces.h"

namespace fs
{
    class GameContext : Uncopyable
    {
    public:
        static IGameApp* GameApp;

        static IInputSystem* Input;
        static IAudioSystem* Audio;
        static IGraphicsSystem* Graphics;
        static IPhysicsSystem* Physics;
        static INetworkSystem* Network;
        static IResourceSystem* Resource;

    };
}

#endif
