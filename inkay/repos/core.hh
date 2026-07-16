#pragma once

#include <string>

namespace Inkay {
    namespace Repos {
        namespace Web {
            extern std::string JuxtFileURL;
            extern std::string RoseWMSFileURL;
            extern std::string RoseWPSFileURL;
            extern std::string NicoWMSFileURL;
            extern std::string NicoWPSFileURL;
            extern std::string SpfnWMSFileURL;
            extern std::string SpfnWPSFileURL;
        }

        void Init(void);
        void AboutBrowserURL(void);
        void Shutdown(void);
    }
}