#include <coreinit/memdefaultheap.h>
#include "eg2/libs/typedefs.h"

namespace Heap {
    namespace MEM2 {
        void* Alloc(u32 size, u32 alignment) {
            if (alignment < 4) alignment = 4;
            return MEMAllocFromDefaultHeapEx(size, alignment);
        }

        void Free(void* block) { MEMFreeToDefaultHeap(block); }
    }
}