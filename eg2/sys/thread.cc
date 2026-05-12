#include <coreinit/thread.h>
#include "eg2/libs/typedefs.h"
#include "eg2/sys/time.hh"

namespace SYS {
    namespace Thread {
        void Yield(void) { OSYieldThread(); }
        void Sleep(u32 ms) { SYS::Time::MakeOSWait(ms); }
    }
}