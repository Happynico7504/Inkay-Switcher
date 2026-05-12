#include <gx2/display.h>
#include "eg2/libs/typedefs.h"

namespace Render {
    f32 gTargetWidth  = 0.0f;
    f32 gTargetHeight = 0.0f;

    void GetSize(bool drc, u32* width, u32* height, GX2TVRenderMode* mode) {
        if (drc) {
            *width  = 854;
            *height = 480;
            if (mode) *mode = GX2_TV_RENDER_MODE_WIDE_480P;
            return;
        }

        GX2TVScanMode scan = GX2GetSystemTVScanMode();
        GX2AspectRatio ratio = GX2GetSystemTVAspectRatio();

        switch (scan) {
            case GX2_TV_SCAN_MODE_480I:
            case GX2_TV_SCAN_MODE_480P:
                if (ratio == GX2_ASPECT_RATIO_16_9) {
                    *width  = 854;
                    *height = 480;
                    if (mode) *mode = GX2_TV_RENDER_MODE_WIDE_480P;
                } else {
                    *width  = 640;
                    *height = 480;
                    if (mode) *mode = GX2_TV_RENDER_MODE_STANDARD_480P;
                }
                break;
            case GX2_TV_SCAN_MODE_720P:
                *width  = 1280;
                *height = 720;
                if (mode) *mode = GX2_TV_RENDER_MODE_WIDE_720P;
                break;
            case GX2_TV_SCAN_MODE_1080I:
            case GX2_TV_SCAN_MODE_1080P:
                *width  = 1920;
                *height = 1080;
                if (mode) *mode = GX2_TV_RENDER_MODE_WIDE_1080P;
                break;
            case GX2_TV_SCAN_MODE_NONE:
                *width  = 0;
                *height = 0;
                if (mode) *mode = GX2_TV_RENDER_MODE_DISABLED;
                break;
            default:
                *width  = 1280;
                *height = 720;
                if (mode) *mode = GX2_TV_RENDER_MODE_WIDE_720P;
                break;
        }
    }

    void Update(bool drc) {
        u32 width, height;
        GX2TVRenderMode mode;
        GetSize(drc, &width, &height, &mode);

        gTargetWidth  = (f32)width;
        gTargetHeight = (f32)height;
    }
}