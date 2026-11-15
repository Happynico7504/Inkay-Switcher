#pragma once

#include <string>
#include <vector>
#include "eg2/libs/typedefs.h"

namespace Inkay {
    namespace Repos {
        enum class SourceType { GithubZip, GithubRaw, DirectRaw };

        struct SourceInfo {
            std::string id;
            std::string name;
            u32 color{0x00C900FF};
            SourceType type{SourceType::GithubZip};
            std::string repo;      // "owner/name", resolved via GitHub API for GithubZip/GithubRaw
            std::string zipUrl;    // resolved release asset URL, GithubZip only
            std::string wmsUrl;    // resolved (GithubRaw) or manifest-provided (DirectRaw) module URL
            std::string wpsUrl;    // resolved (GithubRaw) or manifest-provided (DirectRaw) plugin URL

            // Integrity verification (optional, from manifest)
            std::string zipHash;   // SHA256 hash of zip file (hex)
            std::string wmsHash;   // SHA256 hash of WMS module (hex)
            std::string wpsHash;   // SHA256 hash of WPS plugin (hex)
            u32 zipSize{0};        // expected size in bytes, 0 = unknown
            u32 wmsSize{0};        // expected size in bytes, 0 = unknown
            u32 wpsSize{0};        // expected size in bytes, 0 = unknown
        };

        extern std::vector<SourceInfo> Sources;
        extern bool SourcesLoaded;

        void Init(void);
        void AboutBrowserURL(void);
        void Shutdown(void);
    }
}
