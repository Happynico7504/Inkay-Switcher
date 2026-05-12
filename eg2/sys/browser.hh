#pragma once

#include <string>

namespace SYS {
    namespace Apps {
        std::string BrowserEncodeURL(const std::string& url);
        bool IsValidURL(const char* url);
        s32 BrowserOpen(const char* url);
        s32 BrowserOpenViewer(const char* url);
        s32 BrowserOpenCallback(const char* url, const char* callbackUrl, bool disableHbm);
    }
}