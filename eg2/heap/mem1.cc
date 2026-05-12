#include <coreinit/memfrmheap.h>
#include <coreinit/memexpheap.h>
#include "eg2/libs/typedefs.h"
#include "eg2/heap/mem2.hh"

namespace Heap {
    namespace MEM1 {
        static MEMHeapHandle HeapMEM1 = nullptr;

        bool Init(void) {
            MEMHeapHandle heap = MEMGetBaseHeapHandle(MEM_BASE_HEAP_MEM1);

            if (!MEMRecordStateForFrmHeap(heap, GX2_FRAME_HEAP)) return false;

            u32 size = MEMGetAllocatableSizeForFrmHeapEx(heap, 4);
            if (!size) return false;

            void* base = MEMAllocFromFrmHeapEx(heap, size, 4);
            if (!base) return false;

            HeapMEM1 = MEMCreateExpHeapEx(base, size, 0);
            if (!HeapMEM1) return false;

            return true;
        }

        void* Alloc(u32 size, u32 alignment) {
            if (!HeapMEM1) return nullptr;
            if (alignment < 4) alignment = 4;

            void* block = MEMAllocFromExpHeapEx(HeapMEM1, size, alignment);
            return block;
        }

        void Free(void *block) {
            if (!HeapMEM1) return;
            MEMFreeToExpHeap(HeapMEM1, block);
        }

        bool Destroy(void) {
            MEMHeapHandle heap = MEMGetBaseHeapHandle(MEM_BASE_HEAP_MEM1);

            if (HeapMEM1) {
                MEMDestroyExpHeap(HeapMEM1);
                HeapMEM1 = nullptr;
            }

            MEMFreeByStateToFrmHeap(heap, GX2_FRAME_HEAP);
            return true;
        }
    }
}
