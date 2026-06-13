#pragma once

#include <string>

namespace Inkay {
    namespace Repos {
        namespace Web {
            extern std::string JuxtFileURL;
            extern std::string RoseFileURL;
            extern std::string NicoFileURL;
        }

        void Init(void);
        void AboutBrowserURL(void);
        void Shutdown(void);
    }
}