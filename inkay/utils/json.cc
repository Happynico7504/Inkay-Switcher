#include "eg2/libs/nlohmannjson.hh"
#include "eg2/sys/report.hh"
#include "eg2/network/curl.hh"
#include "inkay/repos/core.hh"
#include <string>
#include <cstdlib>

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

        void ParseGitHubRawFileURLs(const char* url, std::string& outWMSUrl, std::string& outWPSUrl) {
            if (!Network::SetConnection(url)) {
                SYS::Report::Log("Inkay::JSON::ParseGitHubRawFileURLs(): curl fail for: %s\n", url);
                return;
            }

            auto j = nlohmann::json::parse(Network::GetLastResponse(), nullptr, false);
            if (j.is_discarded() || !j.contains("assets")) return;

            for (const auto& asset : j["assets"]) {
                if (!asset.contains("name") || !asset.contains("browser_download_url")) continue;
                std::string name = asset["name"];
                std::string dlUrl = asset["browser_download_url"];
                if (name.find(".wms") != std::string::npos) outWMSUrl = dlUrl;
                if (name.find(".wps") != std::string::npos) outWPSUrl = dlUrl;
            }
        }

        static bool ParseHexColor(const std::string& str, u32& outColor) {
            std::string hex = str;
            if (hex.rfind("0x", 0) == 0 || hex.rfind("0X", 0) == 0) hex = hex.substr(2);
            if (!hex.empty() && hex[0] == '#') hex = hex.substr(1);

            if (hex.size() != 6 && hex.size() != 8) return false;
            if (hex.size() == 6) hex += "FF";

            char* end = nullptr;
            unsigned long v = strtoul(hex.c_str(), &end, 16);
            if (!end || *end != '\0') return false;

            outColor = (u32)v;
            return true;
        }

        bool ParseSourcesManifest(const char* url, std::vector<Inkay::Repos::SourceInfo>& outSources) {
            outSources.clear();

            if (!Network::SetConnection(url)) {
                SYS::Report::Log("Inkay::JSON::ParseSourcesManifest(): curl fail for: %s\n", url);
                return false;
            }

            SYS::Report::Log("Inkay::JSON::ParseSourcesManifest(): HTTP code: %ld\n", Network::GetNumericCode());

            auto j = nlohmann::json::parse(Network::GetLastResponse(), nullptr, false);
            if (j.is_discarded() || !j.contains("sources") || !j["sources"].is_array()) return false;

            for (const auto& entry : j["sources"]) {
                if (!entry.contains("id") || !entry.contains("name") || !entry.contains("type")) continue;

                Inkay::Repos::SourceInfo src;
                src.id = entry["id"].get<std::string>();
                src.name = entry["name"].get<std::string>();

                std::string type = entry["type"].get<std::string>();
                if (type == "github_zip") src.type = Inkay::Repos::SourceType::GithubZip;
                else if (type == "github_raw") src.type = Inkay::Repos::SourceType::GithubRaw;
                else if (type == "direct_raw") src.type = Inkay::Repos::SourceType::DirectRaw;
                else {
                    SYS::Report::Log("Inkay::JSON::ParseSourcesManifest(): unknown type '%s' for source '%s', skipping\n", type.c_str(), src.id.c_str());
                    continue;
                }

                if (entry.contains("color") && entry["color"].is_string()) {
                    u32 color;
                    if (ParseHexColor(entry["color"].get<std::string>(), color)) src.color = color;
                }

                if (entry.contains("repo")) src.repo = entry["repo"].get<std::string>();
                if (entry.contains("wms_url")) src.wmsUrl = entry["wms_url"].get<std::string>();
                if (entry.contains("wps_url")) src.wpsUrl = entry["wps_url"].get<std::string>();

                if ((src.type == Inkay::Repos::SourceType::GithubZip || src.type == Inkay::Repos::SourceType::GithubRaw) && src.repo.empty()) {
                    SYS::Report::Log("Inkay::JSON::ParseSourcesManifest(): source '%s' missing 'repo', skipping\n", src.id.c_str());
                    continue;
                }

                if (src.type == Inkay::Repos::SourceType::DirectRaw && (src.wmsUrl.empty() || src.wpsUrl.empty())) {
                    SYS::Report::Log("Inkay::JSON::ParseSourcesManifest(): source '%s' missing wms_url/wps_url, skipping\n", src.id.c_str());
                    continue;
                }

                outSources.push_back(std::move(src));
            }

            return !outSources.empty();
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