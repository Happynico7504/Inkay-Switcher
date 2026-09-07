#include "eg2/network/curl.hh"
#include "eg2/sys/report.hh"
#include "eg2/sys/browser.hh"
#include "inkay/repos/core.hh"
#include "inkay/repos/versions.hh"
#include "inkay/utils/json.hh"
#include <string>

namespace Inkay {
    namespace Repos {
        std::vector<SourceInfo> Sources;
        bool SourcesLoaded = false;

        // Served from the dedicated Inkay-Switcher-Config repo so the source list can be
        // updated without shipping a new app release. Update the branch/path if that repo's
        // layout ends up different.
        static const char* SourcesManifestURL = "https://raw.githubusercontent.com/Happynico7504/Inkay-Switcher-Config/main/sources.json";

        static const char* AromaAPI = "https://aroma.foryour.cafe/api/check_versions";
        static const char* BrowserURL = "https://revivetendo.nicochristmann.net";

        static std::string GitHubReleasesAPI(const std::string& repo) {
            return "https://api.github.com/repos/" + repo + "/releases/latest";
        }

        static void ResolveSource(SourceInfo& src) {
            switch (src.type) {
                case SourceType::GithubZip: {
                    std::string api = GitHubReleasesAPI(src.repo);
                    Inkay::JSON::ParseGitHubDownloadURL(api.c_str(), src.zipUrl);
                    break;
                }

                case SourceType::GithubRaw: {
                    std::string api = GitHubReleasesAPI(src.repo);
                    Inkay::JSON::ParseGitHubRawFileURLs(api.c_str(), src.wmsUrl, src.wpsUrl);
                    break;
                }

                case SourceType::DirectRaw:
                    // wmsUrl/wpsUrl already come straight from the manifest.
                    break;
            }
        }

        void Init(void) {
            Network::GiveCertCA("fs:/vol/content/shoumeisho/CA.pem");
            Network::SetUserAgent("Mozilla/5.0 (Inkay/1.0; WiiU; curl)");
            Network::SetHeader("Accept: application/vnd.github+json");
            Network::SetHeader("X-GitHub-Api-Version: 2022-11-28");

            SourcesLoaded = Inkay::JSON::ParseSourcesManifest(SourcesManifestURL, Sources);

            if (!SourcesLoaded) {
                SYS::Report::Log("Inkay::Repos::Init(): failed to load source manifest from %s\n", SourcesManifestURL);
            }

            for (auto& src : Sources) ResolveSource(src);

            Inkay::JSON::ParseAromaSHA1(AromaAPI, Inkay::Versions::WMS::JuxtSHA1, Inkay::Versions::WPS::JuxtSHA1);

            bool ok = Inkay::Versions::VerifyJuxtInstall();

            SYS::Report::Log("\n-----------------------------------\n");
            SYS::Report::Log("Inkay Info\n");
            SYS::Report::Log("-----------------------------------\n");
            SYS::Report::Log("Local WMS: %s\n", Inkay::Versions::WMS::LocalSHA1.c_str());
            SYS::Report::Log("Local WPS: %s\n", Inkay::Versions::WPS::LocalSHA1.c_str());
            SYS::Report::Log("Juxt WMS: %s\n", Inkay::Versions::WMS::JuxtSHA1.c_str());
            SYS::Report::Log("Juxt WPS: %s\n", Inkay::Versions::WPS::JuxtSHA1.c_str());
            SYS::Report::Log("Juxt PLG: %s\n\n", ok ? "true" : "false");

            for (const auto& src : Sources) {
                SYS::Report::Log("Source '%s' (%s): zip=%s wms=%s wps=%s\n",
                    src.id.c_str(), src.name.c_str(), src.zipUrl.c_str(), src.wmsUrl.c_str(), src.wpsUrl.c_str());
            }

            SYS::Report::Log("-----------------------------------\n\n");
        }

        void AboutBrowserURL(void) { SYS::Apps::BrowserOpenViewer(BrowserURL); }

        void Shutdown(void) {
            Sources.clear();

            if (!Inkay::Versions::WMS::LocalSHA1.empty()) Inkay::Versions::WMS::LocalSHA1.clear();
            if (!Inkay::Versions::WMS::JuxtSHA1.empty()) Inkay::Versions::WMS::JuxtSHA1.clear();
            if (!Inkay::Versions::WPS::LocalSHA1.empty()) Inkay::Versions::WPS::LocalSHA1.clear();
            if (!Inkay::Versions::WPS::JuxtSHA1.empty()) Inkay::Versions::WPS::JuxtSHA1.clear();
        }
    }
}
