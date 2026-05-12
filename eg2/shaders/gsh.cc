#include "eg2/shaders/latte.hh"
#include "eg2/shaders/gsh.hh"

namespace Shaders {
    namespace GSH {
        Shaders::Latte::Shader GSHColorGroup;
        Shaders::Latte::Shader GSHTexturesGroup;
    }

    void Bind(void) {
        Latte::InitMEM(&GSH::GSHColorGroup, GSH::GSHColor);
        Latte::InitMEM(&GSH::GSHTexturesGroup, GSH::GSHTextures);

        Latte::Attr ColorAttributes[] = {
            {"aPosition", 0, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32_32},
            {"aColour",   1, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32_32_32}
        };

        Latte::Attr TEXAttributes[] = {
            {"aPosition", 0, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32_32},
            {"aTexCoord", 1, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32},
            {"aColour",   2, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32_32_32}
        };

        for (int i = 0; i < 3; ++i) Latte::Attribute(&GSH::GSHTexturesGroup, TEXAttributes[i].name, TEXAttributes[i].buffer, TEXAttributes[i].offset, TEXAttributes[i].format);
        for (int i = 0; i < 2; ++i) Latte::Attribute(&GSH::GSHColorGroup, ColorAttributes[i].name, ColorAttributes[i].buffer, ColorAttributes[i].offset, ColorAttributes[i].format);

        Latte::Fetch(&GSH::GSHColorGroup);
        Latte::Fetch(&GSH::GSHTexturesGroup);
    }
}