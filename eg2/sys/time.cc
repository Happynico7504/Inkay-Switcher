#include <coreinit/time.h>
#include <coreinit/thread.h>
#include "eg2/libs/typedefs.h"
#include "eg2/sys/time.hh"
#include "eg2/sys/thread.hh"

namespace SYS {
    namespace Time {
        DeltaTimer::DeltaTimer(void) { Reset(); }
        void DeltaTimer::Reset(void) { lastTicks = OSGetTime(); }

        f32 DeltaTimer::GetDelta(void) {
            ClockTime current = OSGetTime();
            ClockTime duration = current - lastTicks;
            lastTicks = current;
            return (f32)OSTicksToMilliseconds(duration);
        }

        void SleepTicks(ClockTime ticks) { OSSleepTicks((ClockTime)ticks); }

        ClockTime GetTicks(void) { return OSGetTime(); }
        f32 TicksToMillis(ClockTime ticks) { return (f32)OSTicksToMilliseconds(ticks); }
        f32 GetMillis(void) { return TicksToMillis(OSGetTime()); }

        u64 ClockSpeedTimer(void) { return OSGetSystemInfo()->busClockSpeed / 4; }

        u64 SecondsToTicks(u64 val) { return val * ClockSpeedTimer(); }
        u64 MillisecondsToTicks(u64 val) { return (val * ClockSpeedTimer()) / 1000ull; }
        u64 MicrosecondsToTicks(u64 val) { return (val * ClockSpeedTimer()) / 1000000ull; }

        u64 NanosecondsToTicks(u64 val) { return (val * (ClockSpeedTimer() / 31250ull)) / 32000ull; }
        u64 TicksToNanoseconds(u64 val) { return (val * 32000ull) / (ClockSpeedTimer() / 31250ull); }

        u64 TicksToSeconds(u64 val) { return val / ClockSpeedTimer(); }
        u64 TicksToMilliseconds(u64 val) { return (val * 1000ull) / ClockSpeedTimer(); }
        u64 TicksToMicroseconds(u64 val) { return (val * 1000000ull) / ClockSpeedTimer(); }

        void MakeOSWait(u32 ms) { 
            if (ms == 0) {
                SYS::Thread::Yield();
                return;
            }

            SleepTicks(MillisecondsToTicks(ms));
        }
    }
}