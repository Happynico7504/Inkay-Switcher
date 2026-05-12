#pragma once

#include <gx2/texture.h>

namespace GX2 {
    void AllocateTexture(GX2Texture* texture, bool allocateInMEM1);
    void DeallocateTexture(GX2Texture* texture);
}