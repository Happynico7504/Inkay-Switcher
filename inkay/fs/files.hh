#pragma once

#include "eg2/libs/typedefs.h"
#include <string>

namespace Inkay {
    namespace Files {
        std::string GetWMSPath(void);
        std::string GetWPSPath(void);
        bool WriteWMS(const void* data, usize size);
        bool WriteWPS(const void* data, usize size);
    }
}