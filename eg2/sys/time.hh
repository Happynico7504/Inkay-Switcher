#pragma once

#include <coreinit/time.h>
#include "eg2/libs/typedefs.h"

typedef OSTime ClockTime;

namespace SYS {
    namespace Time {
        class DeltaTimer {
            public:
                DeltaTimer(void);
                void Reset(void);
                f32 GetDelta(void);
            private:
                OSTime lastTicks;
        };

        void SleepTicks(ClockTime ticks);

        f32 GetMillis(void);
        OSTime GetTicks(void);
        f32 TicksToMillis(OSTime ticks);
        void MakeOSWait(u32 ms);

        u64 ClockSpeedTimer(void);

        u64 SecondsToTicks(u64 val);
        u64 MillisecondsToTicks(u64 val);
        u64 MicrosecondsToTicks(u64 val);

        u64 NanosecondsToTicks(u64 val);
        u64 TicksToNanoseconds(u64 val);

        u64 TicksToSeconds(u64 val);
        u64 TicksToMilliseconds(u64 val);
        u64 TicksToMicroseconds(u64 val);
    }
}