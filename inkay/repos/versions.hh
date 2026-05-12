#pragma once

#include "eg2/libs/typedefs.h"
#include <string>

namespace Inkay {
    namespace Versions {
        namespace WMS {
            extern std::string LocalSHA1;
            extern std::string JuxtSHA1; 
        }

        namespace WPS {
            extern std::string LocalSHA1;
            extern std::string JuxtSHA1; 
        }

        enum class LocalFileState : u8 {
            Missing,
            PresentButInvalid,
            ValidJuxt,
            ValidUnknown
        };

        extern LocalFileState FileState;

        bool VerifyJuxtInstall(void);
    }
}