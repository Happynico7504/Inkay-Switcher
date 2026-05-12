#pragma once

#include "eg2/libs/typedefs.h"

namespace SYS {
    namespace Thread {
        void Yield(void);
        void Sleep(u32 ms);
    }
}