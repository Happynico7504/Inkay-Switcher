#pragma once

#include "eg2/libs/typedefs.h"

namespace Heap {
    namespace MEM1 {
        bool Init(void);
        void* Alloc(u32 size, u32 alignment);
        void Free(void* block);
        bool Destroy(void);
    }
}