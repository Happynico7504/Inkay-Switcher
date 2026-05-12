#pragma once

#include "eg2/libs/typedefs.h"
#include "eg2/render/frames.hh"

namespace Render {
    void Init(void);
    bool IsRunning(void);
    void Shutdown(void);
    void ExitToMenu(void);
    void StopRunning(void);
    void SoftRebootConsole(void);
    void FullyRebootConsole(void);
}