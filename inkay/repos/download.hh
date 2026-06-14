#pragma once

#include <atomic>
#include <string>

enum DownloadState {
    DOWNLOADSTATE_SELECT,
    DOWNLOADSTATE_HBAS_WARNING,
    DOWNLOADSTATE_SELECT_ENV,
    DOWNLOADSTATE_DOWNLOADING,
    DOWNLOADSTATE_ERROR,
    DOWNLOADSTATE_FINISHED
};

namespace Inkay {
    namespace Download {
        enum class Source { None, Juxt, Rose, Nico };

        extern std::string LastError;
        extern std::atomic<bool> HasError;
        extern Source Pending;
        extern std::string SelectedEnvironment;
        extern std::atomic<DownloadState> State;

        void JuxtDownload(void);
        void RoseDownload(void);
        void NicoDownload(void);
        void UpdateDownloads(void);
    }
}
