#pragma once

#include "eg2/libs/typedefs.h"

namespace Inkay {
    namespace GUI {
        enum Colors : u32 {
            Default = 0x00C900FF,
            Juxt = 0x8C50DCFF,
            NoInkay = 0x282828FF,
            Browser = 0x1A0DABFF,
            Black = 0x000000FF,
            Holder = 0xD5D5D5FF
        };

        extern const u32 BgColor;

        void Init(void);
        void Render(void);
        void Update(void);
        void Shutdown(void);
    }
}