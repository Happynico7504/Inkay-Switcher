#pragma once

#include <string>

namespace Inkay {
    namespace JSON {
        void ParseGitHubTag(const char* url, std::string& outTag);
        void ParseGitHubDownloadURL(const char* url, std::string& outUrl);
        void ParseAromaSHA1(const char* url, std::string& outWmsSHA1, std::string& outWpsSHA1);
    }
}