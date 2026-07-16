#include "eg2/network/curl.hh"
#include "eg2/sys/report.hh"
#include "eg2/sys/browser.hh"
#include "inkay/repos/core.hh"
#include "inkay/repos/versions.hh"
#include "inkay/utils/json.hh"
#include <string>

namespace Inkay {
    namespace Repos {
        namespace Web {
            std::string JuxtFileURL;
            std::string RoseWMSFileURL;
            std::string RoseWPSFileURL;
            std::string NicoWMSFileURL;
            std::string NicoWPSFileURL;
            std::string SpfnWMSFileURL;
            std::string SpfnWPSFileURL;
        }

        static const char* JuxtREPO = "https://api.github.com/repos/PretendoNetwork/Inkay/releases/latest";
        static const char* RoseREPO = "https://api.github.com/repos/Project-Rose/Inkay-Roseverse/releases/latest";
        static const char* NicoREPO  = "https://api.github.com/repos/Happynico7504/Inkay-NicoChristmann/releases/latest";
        static const char* SpfnWMSURL = "https://git.spbr.net/spacebar/inkay/releases/download/latest/Inkay-spfn.wms";
        static const char* SpfnWPSURL = "https://git.spbr.net/spacebar/inkay/releases/download/latest/Inkay-spfn.wps";
        static const char* AromaAPI = "https://aroma.foryour.cafe/api/check_versions";
        static const char* BrowserURL = "https://projectrose.cafe/roseverse_installer/help";

        void Init(void) {
            Network::GiveCertCA("fs:/vol/content/shoumeisho/CA.pem");
            Network::SetUserAgent("Mozilla/5.0 (Inkay/1.0; WiiU; curl)");
            Network::SetHeader("Accept: application/vnd.github+json");
            Network::SetHeader("X-GitHub-Api-Version: 2022-11-28");

            Inkay::JSON::ParseGitHubDownloadURL(JuxtREPO, Web::JuxtFileURL);
            Inkay::JSON::ParseGitHubRawFileURLs(RoseREPO, Web::RoseWMSFileURL, Web::RoseWPSFileURL);
            Inkay::JSON::ParseGitHubRawFileURLs(NicoREPO,  Web::NicoWMSFileURL, Web::NicoWPSFileURL);

            Web::SpfnWMSFileURL = SpfnWMSURL;
            Web::SpfnWPSFileURL = SpfnWPSURL;

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
            SYS::Report::Log("Juxt URL: %s\n", Web::JuxtFileURL.c_str());
            SYS::Report::Log("Rose WMS: %s\n", Web::RoseWMSFileURL.c_str());
            SYS::Report::Log("Rose WPS: %s\n", Web::RoseWPSFileURL.c_str());
            SYS::Report::Log("Nico WMS: %s\n", Web::NicoWMSFileURL.c_str());
            SYS::Report::Log("Nico WPS: %s\n", Web::NicoWPSFileURL.c_str());
            SYS::Report::Log("Spfn WMS: %s\n", Web::SpfnWMSFileURL.c_str());
            SYS::Report::Log("Spfn WPS: %s\n", Web::SpfnWPSFileURL.c_str());
            SYS::Report::Log("-----------------------------------\n\n");
        }

        void AboutBrowserURL(void) { SYS::Apps::BrowserOpenViewer(BrowserURL); }

        void Shutdown(void) {
            if (!Web::JuxtFileURL.empty()) Web::JuxtFileURL.clear();
            if (!Web::RoseWMSFileURL.empty()) Web::RoseWMSFileURL.clear();
            if (!Web::RoseWPSFileURL.empty()) Web::RoseWPSFileURL.clear();
            if (!Web::NicoWMSFileURL.empty()) Web::NicoWMSFileURL.clear();
            if (!Web::NicoWPSFileURL.empty()) Web::NicoWPSFileURL.clear();
            if (!Web::SpfnWMSFileURL.empty()) Web::SpfnWMSFileURL.clear();
            if (!Web::SpfnWPSFileURL.empty()) Web::SpfnWPSFileURL.clear();

            if (!Inkay::Versions::WMS::LocalSHA1.empty()) Inkay::Versions::WMS::LocalSHA1.clear();
            if (!Inkay::Versions::WMS::JuxtSHA1.empty()) Inkay::Versions::WMS::JuxtSHA1.clear();
            if (!Inkay::Versions::WPS::LocalSHA1.empty()) Inkay::Versions::WPS::LocalSHA1.clear();
            if (!Inkay::Versions::WPS::JuxtSHA1.empty()) Inkay::Versions::WPS::JuxtSHA1.clear();
        }
    }
}