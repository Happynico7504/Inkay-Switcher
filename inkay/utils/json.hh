#pragma once

#include <string>

namespace Inkay {
    namespace JSON {
        void ParseGitHubTag(const char* url, std::string& outTag);
        void ParseGitHubDownloadURL(const char* url, std::string& outUrl);
        void ParseGitHubRawFileURLs(const char* url, std::string& outWMSUrl, std::string& outWPSUrl);
        void ParseAromaSHA1(const char* url, std::string& outWmsSHA1, std::string& outWpsSHA1);
    }
}