#pragma once

#include <nsysnet/_socket.h>

#include "eg2/libs/typedefs.h"

namespace Network {
    namespace Socket {
        extern int RPLWRAP(set_resolver_allocator)(void *(*new_alloc)(u32), void (*new_free)(void *));

        extern void Init(void);
        extern void Finish(void);
        extern int LastError(void);
        extern char *InetNtoA(in_addr in);
        extern int ResolverRegisterAllocator(void* (*alloc_func)(u32), void (*free_func)(void*));
    }
}