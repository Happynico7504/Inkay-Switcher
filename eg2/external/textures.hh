#pragma once

#include <gx2/texture.h>
#include "eg2/libs/typedefs.h"

namespace IO {
    GX2Texture* JPGRead(const u8* data, usize length);
    GX2Texture* PNGRead(const u8* data, usize length);
    GX2Texture* GIFRead(const u8* data, usize length, int frameIndex);
}