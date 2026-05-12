#pragma once

#include <gx2/texture.h>
#include "eg2/libs/typedefs.h"

typedef enum GX2CompMapType {
    GX2_COMPMAP_TYPE_TEXTURE,
    GX2_COMPMAP_TYPE_H264,
    GX2_COMPMAP_TYPE_LOOSE,
} GX2CompMapType;

namespace GX2 {
    void CreateTexture(GX2Texture* tex, u32 w, u32 h, u32 depth, u32 nMips, GX2CompMapType CMtype, GX2SurfaceFormat format, GX2SurfaceDim dim, GX2TileMode TM, u32 swizzle, bool m1);
    void UnloadTexture(GX2Texture* texture);
    void DestroyTexture(GX2Texture** tex);
}