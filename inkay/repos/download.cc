#include "eg2/sys/report.hh"
#include "eg2/network/curl.hh"
#include "eg2/external/core.hh"
#include "inkay/repos/core.hh"
#include "inkay/repos/download.hh"
#include "inkay/repos/verify.hh"
#include "inkay/fs/zip.hh"
#include "inkay/fs/files.hh"
#include <vector>
#include <atomic>
#include <thread>
#include <chrono>

namespace Inkay {
    namespace Download {
        std::string LastError;
        std::string SelectedEnvironment;
        std::atomic<bool> HasError{false};
        std::atomic<DownloadState> State{DOWNLOADSTATE_SELECT};
        int PendingIndex{-1};

        static std::vector<u8> gFileData;
        static std::string gDownloadName;
        static std::atomic<bool> gPendingWrite{false};
        static bool gIsRawDownload{false};
        static int gRawPhase{0};
        static int gRetryCount{0};
        static const int kMaxRetries = 3;
        static const int kRetryDelayMs = 100;

        // Real .wms/.wps modules are ELF binaries; anything else (e.g. a GitHub
        // rate-limit/error JSON body that slipped past the HTTP status check) is bogus.
        static const usize kMinRawDownloadSize = 4096;

        static void SetError(const std::string& msg) {
            LastError = msg;
            HasError.store(true, std::memory_order_release);
            State.store(DOWNLOADSTATE_ERROR, std::memory_order_release);
            SYS::Report::Log("Inkay::Download::SetError(): %s\n", msg.c_str());
        }

        static bool LooksLikeELF(const std::vector<u8>& data) {
            return data.size() >= 4 && data[0] == 0x7F && data[1] == 'E' && data[2] == 'L' && data[3] == 'F';
        }

        static std::string GetExpectedHash(bool isRaw, int phase) {
            if (PendingIndex < 0 || (usize)PendingIndex >= Inkay::Repos::Sources.size()) {
                return "";
            }
            const Inkay::Repos::SourceInfo& src = Inkay::Repos::Sources[PendingIndex];
            if (isRaw) {
                return (phase == 0) ? src.wmsHash : src.wpsHash;
            }
            return src.zipHash;
        }

        static u32 GetExpectedSize(bool isRaw, int phase) {
            if (PendingIndex < 0 || (usize)PendingIndex >= Inkay::Repos::Sources.size()) {
                return 0;
            }
            const Inkay::Repos::SourceInfo& src = Inkay::Repos::Sources[PendingIndex];
            if (isRaw) {
                return (phase == 0) ? src.wmsSize : src.wpsSize;
            }
            return src.zipSize;
        }

        static bool VerifyDownloadIntegrity() {
            long contentLength = Network::GetContentLength();
            std::string expectedHash = GetExpectedHash(gIsRawDownload, gRawPhase);
            u32 expectedSize = GetExpectedSize(gIsRawDownload, gRawPhase);

            // Only perform strict size validation if Content-Length is known
            // (file system caching or other delays can affect Content-Length accuracy)
            if (contentLength > 0 && expectedSize > 0 && (u32)contentLength != expectedSize) {
                SYS::Report::Log("Inkay::Download: Warning - Content-Length mismatch (expected %u, got %ld)\n",
                               expectedSize, contentLength);
            }

            // Hash verification only if explicitly provided in manifest
            // GitHub sources will have their own verification upstream
            if (!expectedHash.empty()) {
                Verify::VerifyResult result = Verify::VerifyDownload(gFileData, expectedHash, expectedSize);
                if (result.error != Verify::VerifyError::OK) {
                    SetError(gDownloadName + ": Integrity check failed - " + result.errorMsg);
                    return false;
                }
                SYS::Report::Log("Inkay::Download: SHA256 verified successfully for %s\n", gDownloadName.c_str());
                return true;
            }

            // Basic sanity checks (no strict size enforcement without explicit expected size)
            if (gFileData.empty()) {
                SetError(gDownloadName + ": Downloaded file is empty");
                return false;
            }

            return true;
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
                if (Network::WasAllocationFailed()) {
                    SetError(gDownloadName + ": Download failed - memory allocation error");
                } else {
                    // Retry on transient network errors
                    if (gRetryCount < kMaxRetries) {
                        gRetryCount++;
                        int delayMs = kRetryDelayMs * (1 << (gRetryCount - 1));  // exponential backoff
                        SYS::Report::Log("Inkay::Download: Retry %d/%d after %dms\n", gRetryCount, kMaxRetries, delayMs);
                        std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
                        gPendingWrite.store(true, std::memory_order_release);
                        return;
                    }
                    SetError(gDownloadName + ": Download failed - " + Network::GetLastError());
                }
                return;
            }

            gRetryCount = 0;  // Reset retry counter on success

            if (gFileData.size() < 4) {
                SetError(gDownloadName + ": Download too small (" + std::to_string(gFileData.size()) + " bytes)");
                return;
            }

            SYS::Report::Log("Inkay::Download::UpdateDownloads(): %s magic: %02X %02X %02X %02X\n", gDownloadName.c_str(), gFileData[0], gFileData[1], gFileData[2], gFileData[3]);
            SYS::Report::Log("Inkay::Download::UpdateDownloads(): %s %u bytes\n", gDownloadName.c_str(), (unsigned)gFileData.size());

            // Verify integrity before writing
            if (!VerifyDownloadIntegrity()) {
                return;  // SetError already called
            }

            if (gIsRawDownload) {
                if (gFileData.size() < kMinRawDownloadSize) {
                    SetError(gDownloadName + ": Download too small (" + std::to_string(gFileData.size()) + " bytes), likely a bad response instead of the real file");
                    return;
                }

                if (!LooksLikeELF(gFileData)) {
                    SetError(gDownloadName + ": Downloaded file is not a valid module (bad signature)");
                    return;
                }

                if (gRawPhase == 0) {
                    if (!Verify::AtomicWrite(Inkay::Files::GetWMSPath(), gFileData.data(), gFileData.size())) {
                        SetError(gDownloadName + ": Failed to write module file");
                        return;
                    }
                    SYS::Report::Log("Inkay::Download::UpdateDownloads(): %s WMS written\n", gDownloadName.c_str());
                    gRawPhase = 1;

                    if (PendingIndex < 0 || (usize)PendingIndex >= Inkay::Repos::Sources.size()) {
                        SetError(gDownloadName + ": Source no longer available");
                        return;
                    }

                    StartDownload(gDownloadName, Inkay::Repos::Sources[PendingIndex].wpsUrl, true);
                } else {
                    if (!Verify::AtomicWrite(Inkay::Files::GetWPSPath(), gFileData.data(), gFileData.size())) {
                        SetError(gDownloadName + ": Failed to write plugin file");
                        return;
                    }
                    SYS::Report::Log("Inkay::Download::UpdateDownloads(): %s WPS written, install complete\n", gDownloadName.c_str());
                    gRawPhase = 0;
                    State.store(DOWNLOADSTATE_FINISHED, std::memory_order_release);
                }
                return;
            }

            if (gFileData.size() < kMinRawDownloadSize) {
                SetError(gDownloadName + ": Download too small (" + std::to_string(gFileData.size()) + " bytes), likely a bad response instead of the real file");
                return;
            }

            if (!Inkay::ZIP::Extract(gFileData)) {
                SetError(gDownloadName + ": Extract failed");
                return;
            }

            SYS::Report::Log("Inkay::Download::UpdateDownloads(): %s install complete\n", gDownloadName.c_str());
            State.store(DOWNLOADSTATE_FINISHED, std::memory_order_release);
        }

        void StartSourceDownload(int index) {
            if (index < 0 || (usize)index >= Inkay::Repos::Sources.size()) {
                SetError("Invalid source selected");
                return;
            }

            PendingIndex = index;
            gRawPhase = 0;

            const Inkay::Repos::SourceInfo& src = Inkay::Repos::Sources[index];

            switch (src.type) {
                case Inkay::Repos::SourceType::GithubZip:
                    StartDownload(src.name, src.zipUrl, false);
                    break;

                case Inkay::Repos::SourceType::GithubRaw:
                case Inkay::Repos::SourceType::DirectRaw:
                    StartDownload(src.name, src.wmsUrl, true);
                    break;
            }
        }
    }
}
