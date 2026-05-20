#pragma once

#include <gx2/display.h>
#include "eg2/libs/typedefs.h"

struct RenderResolutionAspect { f32 width = 0.0f, height = 0.0f; } gTargetRES;

namespace Render {
    extern f32 gTargetWidth;
    extern f32 gTargetHeight;

    void GetSize(bool drc, u32* width, u32* height, GX2TVRenderMode* mode);
    void Update(bool drc);
}