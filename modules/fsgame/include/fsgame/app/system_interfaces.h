#ifndef FS_SYSTEM_INTERFACES_H 
#define FS_SYSTEM_INTERFACES_H

namespace fs
{
    class ProcessManager;
    class Clock;

    class IGameApp
    {
    public:
        virtual const char* getGameTitle() const = 0;
        virtual const char* getBasePath() const = 0;
        virtual ProcessManager* getProcessManager() = 0;
        virtual const Clock* getClock() const = 0;
        virtual void exit() = 0;
    };

    class IInputSystem
    {
    public:

    };

    class IAudioSystem
    {
    public:

    };

    class IGraphicsSystem
    {
    public:

    };

    class IPhysicsSystem
    {
    public:

    };

    class INetworkSystem
    {
    public:

    };

    class IResourceSystem
    {
    public:

    };
}

#endif
