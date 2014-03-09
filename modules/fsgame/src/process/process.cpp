#include "fsgame/process/process.h"

using namespace fs;

Process::Process() :
    _state(UNINITIALIZED),
    _pChild(nullptr)
{
    
}

Process::~Process()
{
    if(_pChild)
    {
        _pChild->onAbort();
        _pChild.reset();
    }
}

StrongProcessPtr Process::removeChild()
{
    if(_pChild)
    {
        // Don't let child get destroyed when we release it.
        StrongProcessPtr pChild = _pChild;
        _pChild.reset();
        return pChild;
    }

    return StrongProcessPtr();
}

bool Process::isAlive() const 
{ return _state == RUNNING || _state == PAUSED;}

bool Process::isDead() const
{ return _state == SUCCEEDED || _state == FAILED || _state == ABORTED; }

bool Process::isRemoved() const
{ return _state == REMOVED; }

bool Process::isPaused() const
{ return _state == PAUSED; }
