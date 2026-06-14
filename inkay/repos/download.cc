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
        std::string SelectedEnvironment;
        std::atomic<bool> HasError{false};
        std::atomic<DownloadState> State{DOWNLOADSTATE_SELECT};
        Source Pending{Source::None};

        static std::vector<u8> gFileData;
        static std::string gDownloadName;
        static std::atomic<bool> gPendingWrite{false};
        static bool gIsRawDownload{false};
        static int gRawPhase{0};

        static void SetError(const std::string& msg) {
            LastError = msg;
            HasError.store(true, std::memory_order_release);
            State.store(DOWNLOADSTATE_ERROR, std::memory_order_release);
            SYS::Report::Log("Inkay::Download::SetError(): %s\n", msg.c_str());
        }

        static void StartDownload(const std::string& name, const std::string& url, bool isRaw = false) {
            if (url.empty()) {
                SetError(name + ": No release found");
                return;
            }

            gDownloadName = name;
            gIsRawDownload = isRaw;
            gFileData.clear();

            State.store(DOWNLOADSTATE_DOWNLOADING, std::memory_order_release);
            gPendingWrite.store(true, std::memory_order_release);

            Network::DownloadAsync(url, &gFileData);
        }

        void UpdateDownloads(void) {
            if (!gPendingWrite.load(std::memory_order_acquire)) return;
            if (!Network::IsDownloadFinished()) return;

            gPendingWrite.store(false, std::memory_order_release);

            if (!Network::WasDownloadSuccessful()) {
                SetError(gDownloadName + ": Download failed");
                return;
            }

            if (gFileData.empty()) {
                SetError(gDownloadName + ": Download data empty");
                return;
            }

            if (gFileData.size() < 4) {
                SetError(gDownloadName + ": Download too small");
                return;
            }

            SYS::Report::Log("Inkay::Download::UpdateDownloads(): %s magic: %02X %02X %02X %02X\n", gDownloadName.c_str(), gFileData[0], gFileData[1], gFileData[2], gFileData[3]);
            SYS::Report::Log("Inkay::Download::UpdateDownloads(): %s %u bytes\n", gDownloadName.c_str(), (unsigned)gFileData.size());

            if (gIsRawDownload) {
                if (gRawPhase == 0) {
                    Inkay::Files::WriteWMS(gFileData.data(), gFileData.size());
                    SYS::Report::Log("Inkay::Download::UpdateDownloads(): %s WMS written\n", gDownloadName.c_str());
                    gRawPhase = 1;
                    const std::string& wpsUrl = (Pending == Source::Rose)
                        ? Inkay::Repos::Web::RoseWPSFileURL
                        : Inkay::Repos::Web::NicoWPSFileURL;
                    StartDownload(gDownloadName, wpsUrl, true);
                } else {
                    Inkay::Files::WriteWPS(gFileData.data(), gFileData.size());
                    SYS::Report::Log("Inkay::Download::UpdateDownloads(): %s WPS written, install complete\n", gDownloadName.c_str());
                    gRawPhase = 0;
                    State.store(DOWNLOADSTATE_FINISHED, std::memory_order_release);
                }
                return;
            }

            if (!Inkay::ZIP::Extract(gFileData)) {
                SetError(gDownloadName + ": Extract failed");
                return;
            }

            SYS::Report::Log("Inkay::Download::UpdateDownloads(): %s install complete\n", gDownloadName.c_str());
            State.store(DOWNLOADSTATE_FINISHED, std::memory_order_release);
        }

        void JuxtDownload(void) { StartDownload("Juxt", Inkay::Repos::Web::JuxtFileURL); }
        void RoseDownload(void) { gRawPhase = 0; StartDownload("Rose", Inkay::Repos::Web::RoseWMSFileURL, true); }
        void NicoDownload(void) { gRawPhase = 0; StartDownload("Nico", Inkay::Repos::Web::NicoWMSFileURL, true); }
    }
}
