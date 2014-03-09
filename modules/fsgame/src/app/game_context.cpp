#include "fsgame/app/game_context.h"

using namespace fs;

IGameApp* GameContext::GameApp = nullptr;
IInputSystem* GameContext::Input = nullptr;
IAudioSystem* GameContext::Audio = nullptr;
IGraphicsSystem* GameContext::Graphics = nullptr;
IPhysicsSystem* GameContext::Physics = nullptr;
INetworkSystem* GameContext::Network = nullptr;
IResourceSystem* GameContext::Resource = nullptr;
