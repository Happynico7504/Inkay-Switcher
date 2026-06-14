#include "eg2/sys/report.hh"
#include "eg2/network/curl.hh"
#include "eg2/external/core.hh"
#include "inkay/repos/core.hh"
#include "inkay/repos/download.hh"
#include "inkay/fs/zip.hh"
#include "inkay/fs/files.hh"
#include <vector>
#include <atomic>

namespace Inkay {
    namespace Download {
        std::string LastError;
        std::string PendingVersion;
        std::string SelectedEnvironment;
        std::atomic<bool> HasError{false};
        std::atomic<DownloadState> State{DOWNLOADSTATE_SELECT};

        static std::vector<u8> gZipData;
        static std::string gCurrentName;
        static std::atomic<bool> gPendingExtract{false};
        static int gRosePhase{0};

        static void SetError(const std::string& msg) {
            LastError = msg;
            HasError.store(true, std::memory_order_release);
            State.store(DOWNLOADSTATE_ERROR, std::memory_order_release);
            SYS::Report::Log("Inkay::Download::SetError(): %s\n", msg.c_str());
        }

        static void StartDownload(const std::string& name, const std::string& url) {
            if (url.empty()) {
                SetError(name + ": No release found");
                return;
            }

            gCurrentName = name;
            gZipData.clear();

            State.store(DOWNLOADSTATE_DOWNLOADING, std::memory_order_release);
            gPendingExtract.store(true, std::memory_order_release);

            Network::DownloadAsync(url, &gZipData);
        }

        void UpdateDownloads(void) {
            if (!gPendingExtract.load(std::memory_order_acquire)) return;
            if (!Network::IsDownloadFinished()) return;

            gPendingExtract.store(false, std::memory_order_release);

            if (!Network::WasDownloadSuccessful()) {
                SetError("Inkay::Download::UpdateDownloads(): " + gCurrentName + "Download failed");
                return;
            }

            if (gZipData.empty()) {
                SetError("Inkay::Download::UpdateDownloads(): " + gCurrentName + " ZIP data empty");
                return;
            }

            if (gZipData.size() < 4) {
                SetError("Inkay::Download::UpdateDownloads(): " + gCurrentName + " ZIP too small");
                return;
            }

            SYS::Report::Log("Inkay::Download::UpdateDownloads(): %s ZIP magic: %02X %02X %02X %02X\n", gCurrentName.c_str(), gZipData[0], gZipData[1], gZipData[2], gZipData[3]);
            SYS::Report::Log("Inkay::Download::UpdateDownloads(): %s ZIP %u bytes\n", gCurrentName.c_str(), (unsigned)gZipData.size());

            if (gCurrentName == "Rose" || gCurrentName == "Nico") {
                if (gRosePhase == 0) {
                    Inkay::Files::WriteWMS(gZipData.data(), gZipData.size());
                    SYS::Report::Log("Inkay::Download::UpdateDownloads(): %s WMS written\n", gCurrentName.c_str());
                    gRosePhase = 1;
                    const std::string& wpsUrl = (gCurrentName == "Rose")
                        ? Inkay::Repos::Web::RoseWPSFileURL
                        : Inkay::Repos::Web::NicoWPSFileURL;
                    StartDownload(gCurrentName, wpsUrl);
                } else {
                    Inkay::Files::WriteWPS(gZipData.data(), gZipData.size());
                    SYS::Report::Log("Inkay::Download::UpdateDownloads(): %s WPS written, install complete\n", gCurrentName.c_str());
                    gRosePhase = 0;
                    State.store(DOWNLOADSTATE_FINISHED, std::memory_order_release);
                }
                return;
            }

            if (!Inkay::ZIP::Extract(gZipData)) {
                SetError("Inkay::Download::UpdateDownloads(): " + gCurrentName + " Extract failed");
                return;
            }

            SYS::Report::Log("Inkay::Download::UpdateDownloads(): %s install complete\n", gCurrentName.c_str());
            State.store(DOWNLOADSTATE_FINISHED, std::memory_order_release);
        }

        void JuxtDownload(void) { StartDownload("Juxt", Inkay::Repos::Web::JuxtFileURL); }
        void RoseDownload(void) { gRosePhase = 0; StartDownload("Rose", Inkay::Repos::Web::RoseWMSFileURL); }
        void NicoDownload(void) { gRosePhase = 0; StartDownload("Nico", Inkay::Repos::Web::NicoWMSFileURL); }
    }
}
