#pragma once

#include <string>
#include <vector>
#include "inkay/repos/core.hh"

namespace Inkay {
    namespace JSON {
        void ParseGitHubTag(const char* url, std::string& outTag);
        void ParseGitHubDownloadURL(const char* url, std::string& outUrl);
        void ParseGitHubRawFileURLs(const char* url, std::string& outWMSUrl, std::string& outWPSUrl);
        void ParseAromaSHA1(const char* url, std::string& outWmsSHA1, std::string& outWpsSHA1);

        // Fetches and parses the dynamic source manifest (see README/server docs for schema).
        bool ParseSourcesManifest(const char* url, std::vector<Inkay::Repos::SourceInfo>& outSources);
    }
}
