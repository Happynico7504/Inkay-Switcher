#include "eg2/libs/nlohmannjson.hh"
#include "eg2/sys/report.hh"
#include "eg2/network/curl.hh"
#include "inkay/repos/core.hh"
#include <string>

namespace Inkay {
    namespace JSON {
        void ParseGitHubDownloadURL(const char* url, std::string& outUrl) {
            if (!Network::SetConnection(url)) {
                SYS::Report::Log("Inkay::JSON::ParseGitHubDownloadURL(): curl fail for: %s\n", url);
                SYS::Report::Log("Inkay::JSON::ParseGitHubDownloadURL(): HTTP code: %ld\n", Network::GetNumericCode());
                return;
            }

            SYS::Report::Log("Inkay::JSON::ParseGitHubDownloadURL(): HTTP code: %ld\n", Network::GetNumericCode());

            auto j = nlohmann::json::parse(Network::GetLastResponse(), nullptr, false);
            if (j.is_discarded()) return;

            if (!j.contains("assets")) return;

            for (const auto& asset : j["assets"]) {
                if (asset.contains("browser_download_url")) {
                    outUrl = asset["browser_download_url"].get<std::string>();
                    return;
                }
            }
        }

        void ParseAromaSHA1(const char* url, std::string& outWmsSHA1, std::string& outWpsSHA1) {
            if (!Network::SetConnection(url)) {
                SYS::Report::Log("ParseAromaSHA1(): curl fail %s\n", url);
                SYS::Report::Log("Inkay::JSON::ParseAromaSHA1(): HTTP code: %ld\n", Network::GetNumericCode());
                return;
            }

            SYS::Report::Log("Inkay::JSON::ParseAromaSHA1(): HTTP code: %ld\n", Network::GetNumericCode());

            auto j = nlohmann::json::parse(Network::GetLastResponse(), nullptr, false);
            if (j.is_discarded() || !j.is_array()) return;

            for (const auto& root : j) {
                if (!root.contains("category")) continue;

                for (const auto& cat : root["category"]) {
                    if (!cat.contains("packages")) continue;

                    for (const auto& pkg : cat["packages"]) {
                        if (!pkg.contains("id")) continue;
                        if (pkg["id"] != "inkay") continue;
                        if (!pkg.contains("repositories")) continue;

                        for (const auto& repo : pkg["repositories"]) {
                            if (!repo.contains("files")) continue;

                            for (const auto& file : repo["files"]) {
                                if (!file.contains("path") || !file.contains("sha1")) continue;

                                std::string path = file["path"];
                                std::string sha1 = file["sha1"];

                                if (path.find(".wms") != std::string::npos) outWmsSHA1 = sha1;
                                if (path.find(".wps") != std::string::npos) outWpsSHA1 = sha1;
                            }
                        }
                    }
                }
            }
        }
    }
}