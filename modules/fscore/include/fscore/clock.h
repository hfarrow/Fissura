#ifndef FS_CLOCK_H
#define FS_CLOCK_H

#include "fscore/types.h"

namespace fs
{
    class Clock
    {
    public:
        // Must called before a clock can be created.
        static void init();

        explicit Clock(f32 startTimeSeconds = 0.0f);

        u64 getTimeCycles() const;
        f32 calcDeltaSeconds(const Clock& other);

        // Return the elapsed time after scale was applied.
        f32 update(f32 dtRealSeconds);

        void pause();
        void resume();
        bool isPaused() const;
        void setTimeScale(f32 scale);
        f32 getTimeScale() const;
        void singleStep();

    private:
        u64 _timeCycles;
        f32 _timeScale;
        bool _isPaused;

        static f32 s_cyclesPerSecond;

        static inline u64 secondsToCycles(f32 timeSeconds)
        {
            return (u64)(timeSeconds * s_cyclesPerSecond);
        }

        // Warning: Only use to convert small durations into seconds
        static inline f32 cyclesToSeconds(u64 timeCycles)
        {
            return (f32)timeCycles / s_cyclesPerSecond;
        }
    };
}

#endif
