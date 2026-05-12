#pragma once

#include <whb/gfx.h>
#include <gx2/shaders.h>
#include "eg2/libs/typedefs.h"

namespace Shaders {
    namespace Latte {
        struct Attr {
            const char* name;
            u32 buffer;
            u32 offset;
            GX2AttribFormat format;
        };

        typedef struct WHBGfxShaderGroup Shader;

        bool Attribute(Shader *group, const char *name, u32 buffer, u32 offset, GX2AttribFormat format);
        bool Fetch(Shader *group);
        bool Free(Shader *group);
        bool InitMEM(Shader* group, const u8* data);
    }

}