#pragma once

#include <gx2/sampler.h>
#include <gx2/texture.h>
#include "eg2/libs/typedefs.h"

struct VTX { f32 x, y, z; };
struct UV { f32 u, v; };
struct F32Color { f32 r, g, b, a; };
struct Rect { f32 x, y, w, h; };

struct Color {
    u8 r, g, b;
    f32 a;
};

typedef enum RenderingAspectType {
    RENDERRATIO_ASPECT_640x360,
    RENDERRATIO_ASPECT_640x480,
    RENDERRATIO_ASPECT_854x480,
    RENDERRATIO_ASPECT_1280x720,
    RENDERRATIO_ASPECT_1920x1080,
} RenderingAspectType;

namespace Tex {
    struct AspectRatio {
        f32 width;
        f32 height;
    };

    const AspectRatio gAspectRatios[] = {
        { 640.0f, 360.0f },
        { 640.0f, 480.0f },
        { 854.0f, 480.0f },
        { 1280.0f, 720.0f },
        { 1920.0f, 1080.0f },
    };

    void Clean(void);
    F32Color U322F32(u32 hex);
    F32Color U82F32(const Color& c);
    void RawQuadColor(f32 x, f32 y, f32 w, f32 h, f32 r, f32 g, f32 b, f32 a);
    void RawPartialColor(GX2Texture* tex, f32 x, f32 y, f32 width, f32 height, int partialX, int partialY, int partialW, int partialH, f32 r, f32 g, f32 b, f32 a);
    void BackgroundRepeat(GX2Texture* tex);
    void BackgroundRepeatScaled(GX2Texture* tex, f32 sx, f32 sy);
}