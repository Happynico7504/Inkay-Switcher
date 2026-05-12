#include "eg2/network/socket.hh"

namespace Network {
    namespace Socket {
        void Init(void) { socket_lib_init(); }
        void Finish(void) { socket_lib_finish(); }
        int LastError(void) { return RPLWRAP(socketlasterr)(); }
        char *InetNtoA(in_addr in) { return inet_ntoa(in); }
        int ResolverRegisterAllocator(void* (*alloc_func)(u32), void (*free_func)(void*)) { return RPLWRAP(set_resolver_allocator)(alloc_func, free_func); }
    }
}