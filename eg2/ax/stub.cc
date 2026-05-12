#include <sndcore2/core.h>

namespace AX {
    static bool gInitialized = false;

    void Init(void) {
        if (gInitialized) return;

        AXInit(); // Internally calls AXInitWithParams()

        gInitialized = true;
    }

    void Shutdown(void) {
        if (!gInitialized) return;

        AXQuit();
        gInitialized = false;
    }

    bool IsInitialized(void) { return gInitialized; }
}