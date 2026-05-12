#include <whb/gfx.h>
#include "eg2/libs/typedefs.h"
#include "eg2/shaders/latte.hh"
#include "eg2/external/core.hh"

namespace Shaders {
    namespace Latte {
        bool Free(Shader *group) { return WHBGfxFreeShaderGroup(group); }
        bool Fetch(Shader *group) { return WHBGfxInitFetchShader(group); }
        bool Attribute(Shader *group, const char *name, u32 buffer, u32 offset, GX2AttribFormat format) { return WHBGfxInitShaderAttribute(group, name, buffer, offset,format); }
        bool InitMEM(Shader* group, const u8* data) { return WHBGfxLoadGFDShaderGroup(group, 0, (void*)data); }
    }
}