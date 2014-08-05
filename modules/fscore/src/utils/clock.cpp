#include <SDL.h>

#include "fscore/utils/types.h"
#include "fscore/utils/clock.h"

using namespace fs;

f32 Clock::s_cyclesPerSecond = 0;
void Clock::init()
{
    s_cyclesPerSecond = SDL_GetPerformanceFrequency();
}

Clock::Clock(f32 startTimeSeconds) :
    _timeCycles(secondsToCycles(startTimeSeconds)),
    _timeScale(1.0f),
    _isPaused(false)
{

}

u64 Clock::getTimeCycles() const
{
    return _timeCycles;
}

f32 Clock::calcDeltaSeconds(const Clock& other)
{
    u64 dt = _timeCycles - other._timeCycles;
    return cyclesToSeconds(dt);
}

f32 Clock::update(f32 dtRealSeconds)
{
    if(!_isPaused)
    {
        u64 dtScaledCycles = secondsToCycles(dtRealSeconds * _timeScale);
        _timeCycles += dtScaledCycles;
        return cyclesToSeconds(dtScaledCycles);
    }

    return 0;
}

void Clock::pause()
{
    _isPaused = true;
}

void Clock::resume()
{
    _isPaused = false;
}

bool Clock::isPaused() const
{
    return _isPaused;
}

void Clock::setTimeScale(f32 scale)
{
    _timeScale = scale;
}

f32 Clock::getTimeScale() const
{
    return _timeScale;
}

void Clock::singleStep()
{
    if(_isPaused)
    {
        // Add one ideal frame interval
        u64 dtScaledCycles = secondsToCycles((1.0f / 60.0f) * _timeScale);
        _timeCycles += dtScaledCycles;
    }
}

